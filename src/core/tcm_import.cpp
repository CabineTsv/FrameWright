#include "tcm_import.hpp"

#include <algorithm>
#include <cstring>

using namespace geode::prelude;

namespace tcm_import {
namespace {

constexpr size_t HEADER_SIZE = 0x10;
constexpr uint8_t TCBOT_HEADER[HEADER_SIZE] = {
    0x9f, 0x88, 0x89, 0x84, 0x9f, 0x3b, 0x1d, 0xd8, 0xcc, 0xa1, 0x86, 0x8a, 0x88, 0x99, 0x84, 0x00,
};

constexpr size_t META_SIZE = 0x40;

// Sticky-error binary cursor: every read is bounds-checked. Once `bad` is set,
// further reads just return 0 without touching `data`, so callers only need to
// check `reader.bad` once at the end instead of after every single read.
struct Reader {
    std::vector<uint8_t> const& data;
    size_t pos = 0;
    bool bad = false;

    bool hasBytes(size_t n) const {
        return !bad && pos + n <= data.size();
    }

    uint8_t readU8() {
        if (!hasBytes(1)) {
            bad = true;
            return 0;
        }
        return data[pos++];
    }

    uint32_t readU16LE() {
        if (!hasBytes(2)) {
            bad = true;
            return 0;
        }
        uint32_t value = static_cast<uint32_t>(data[pos]) | (static_cast<uint32_t>(data[pos + 1]) << 8);
        pos += 2;
        return value;
    }

    uint32_t readU32LE() {
        if (!hasBytes(4)) {
            bad = true;
            return 0;
        }
        uint32_t value = static_cast<uint32_t>(data[pos]) | (static_cast<uint32_t>(data[pos + 1]) << 8) |
                          (static_cast<uint32_t>(data[pos + 2]) << 16) |
                          (static_cast<uint32_t>(data[pos + 3]) << 24);
        pos += 4;
        return value;
    }

    uint64_t readU64LE() {
        if (!hasBytes(8)) {
            bad = true;
            return 0;
        }
        uint64_t value = 0;
        for (int i = 0; i < 8; i++)
            value |= static_cast<uint64_t>(data[pos + i]) << (8 * i);
        pos += 8;
        return value;
    }

    float readF32LE() {
        uint32_t bits = readU32LE();
        float value;
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }

    // LEB128 varint, matching read_var_u32 in tcm-rs
    uint32_t readVarU32() {
        uint32_t value = 0;
        size_t shift = 0;
        while (true) {
            uint8_t byte = readU8();
            if (bad)
                return 0;
            value |= static_cast<uint32_t>(byte & 0x7F) << shift;
            if ((byte & 0x80) == 0)
                return value;
            shift += 7;
            if (shift >= 32) {
                bad = true;
                return 0;
            }
        }
    }
};

struct MetaInfo {
    uint8_t version = 0;
    float tps = 240.f;
    bool hasSeed = false;
    uint64_t seed = 0;
};

geode::Result<MetaInfo> parseMeta(std::vector<uint8_t> const& data) {
    if (data.size() < HEADER_SIZE + META_SIZE)
        return Err("TCM file is too short");

    Reader reader{data, HEADER_SIZE};
    MetaInfo info;
    info.version = reader.readU8();

    if (info.version == 1) {
        reader.pos = HEADER_SIZE + 4;
        info.tps = reader.readF32LE();
    } else if (info.version == 2) {
        uint8_t flags = data[HEADER_SIZE + 2];
        reader.pos = HEADER_SIZE + 4;
        float tpsOrDt = reader.readF32LE();

        bool tpsInsteadOfDt = (flags & (1 << 1)) != 0;
        info.tps = tpsInsteadOfDt ? tpsOrDt : (tpsOrDt != 0.f ? 1.f / tpsOrDt : 240.f);

        uint64_t seed = reader.readU64LE();
        bool overrideSeed = (flags & 1) != 0;
        info.hasSeed = overrideSeed && seed != 0;
        info.seed = seed;
    } else {
        return Err(fmt::format("Unsupported TCM meta version: {}", info.version));
    }

    if (reader.bad)
        return Err("TCM file is too short");

    return Ok(info);
}

std::vector<input> parseInputsV1(Reader& reader) {
    constexpr uint8_t INPUT_MASK = 0b111;
    constexpr uint8_t PUSH_MASK = 1 << 7;
    constexpr uint8_t PLAYER2_MASK = 1 << 6;

    uint32_t count = reader.readVarU32();
    if (reader.bad)
        return {};

    std::vector<input> segment;
    uint32_t segmentBase = 0;

    for (uint32_t i = 0; i < count; i++) {
        uint32_t frame = reader.readVarU32();
        if (reader.bad)
            break;
        uint8_t byte = reader.readU8();
        if (reader.bad)
            break;

        uint8_t value = byte & INPUT_MASK;
        if (value < 3) {
            uint8_t button = value + 1;
            bool push = (byte & PUSH_MASK) != 0;
            bool player2 = (byte & PLAYER2_MASK) != 0;
            segment.emplace_back(static_cast<uint64_t>(frame - segmentBase), button, player2, push);
        } else if (value <= 5) {
            // Restart / RestartFull / Death: xdBot has no native restart marker and its
            // engine already restarts playback from the first input on every real attempt,
            // so we keep only the inputs recorded since the most recent restart marker,
            // renumbered relative to it.
            segment.clear();
            segmentBase = frame;
        } else {
            reader.bad = true;
            break;
        }
    }

    return segment;
}

enum class ByteBlob { Zero = 0, One = 1, Two = 2, Four = 3 };

std::vector<input> parseInputsV2(Reader& reader) {
    constexpr uint8_t INPUT_MASK = 0b11;
    constexpr uint8_t PUSH_MASK = 1 << 2;
    constexpr uint8_t PLAYER2_MASK = 1 << 3;
    constexpr uint8_t CUSTOM_MASK = 0b11 << 2;
    constexpr uint8_t EXTRA_MASK = 1 << 4;
    constexpr uint8_t DELTA_DATA_MASK = 0b111 << 5;

    std::vector<input> segment;

    uint64_t currentFrame = reader.readVarU32();
    if (reader.bad)
        return segment;
    uint64_t lastDelta = 0;

    enum class Blob { Action, FrameDelta, Tps, Seed };
    Blob nextBlob = Blob::Action;

    ByteBlob deltaBlob = ByteBlob::Zero;
    bool deltaHasMagic = false;
    bool awaitingSeed = false;

    while (true) {
        if (nextBlob == Blob::Action) {
            if (!reader.hasBytes(1))
                break;
            uint8_t byte = reader.readU8();

            uint8_t deltaData = (byte & DELTA_DATA_MASK) >> 5;
            deltaBlob = static_cast<ByteBlob>((deltaData >> 1) & 0b11);
            deltaHasMagic = (deltaData & 1) != 0;

            uint8_t inputData = byte & INPUT_MASK;
            if (inputData > 0) {
                uint8_t button = inputData;
                bool push = (byte & PUSH_MASK) != 0;
                bool player2 = (byte & PLAYER2_MASK) != 0;
                bool swift = (byte & EXTRA_MASK) != 0;

                segment.emplace_back(currentFrame, button, player2, push);
                if (swift)
                    segment.emplace_back(currentFrame, button, player2, !push);

                nextBlob = Blob::FrameDelta;
            } else {
                uint8_t customType = (byte & CUSTOM_MASK) >> 2;
                bool extra = (byte & EXTRA_MASK) != 0;

                if (customType == 3) {
                    // Bugpoint (extra) or Tps change (!extra)
                    nextBlob = extra ? Blob::FrameDelta : Blob::Tps;
                } else {
                    // Restart marker: keep only inputs since the most recent restart.
                    segment.clear();
                    currentFrame = 0;
                    if (extra) {
                        awaitingSeed = true;
                        nextBlob = Blob::Seed;
                    } else {
                        nextBlob = Blob::FrameDelta;
                    }
                }
            }
        } else if (nextBlob == Blob::FrameDelta) {
            uint64_t result;
            if (deltaBlob == ByteBlob::Zero) {
                result = deltaHasMagic ? lastDelta : 0;
            } else {
                size_t needed = deltaBlob == ByteBlob::One ? 1 : (deltaBlob == ByteBlob::Two ? 2 : 4);
                if (!reader.hasBytes(needed))
                    break;

                uint32_t raw = deltaBlob == ByteBlob::One   ? reader.readU8()
                                : deltaBlob == ByteBlob::Two ? reader.readU16LE()
                                                              : reader.readU32LE();
                result = (deltaHasMagic ? lastDelta : 0) + raw;
                if (result != 0)
                    lastDelta = result;
            }

            currentFrame += result;
            nextBlob = Blob::Action;
        } else if (nextBlob == Blob::Tps) {
            if (!reader.hasBytes(4)) {
                reader.bad = true;
                break;
            }
            reader.readF32LE();
            nextBlob = Blob::FrameDelta;
        } else if (nextBlob == Blob::Seed) {
            if (!reader.hasBytes(8)) {
                reader.bad = true;
                break;
            }
            reader.readU64LE();
            awaitingSeed = false;
            nextBlob = Blob::FrameDelta;
        }
    }

    if (awaitingSeed)
        reader.bad = true;

    return segment;
}

} // namespace

geode::Result<BotReplay> importTCM(std::vector<uint8_t> const& data) {
    if (data.size() < HEADER_SIZE || std::memcmp(data.data(), TCBOT_HEADER, HEADER_SIZE) != 0)
        return Err("Not a valid TCM file (header mismatch)");

    auto metaResult = parseMeta(data);
    if (metaResult.isErr())
        return Err(metaResult.unwrapErr());
    auto meta = metaResult.unwrap();

    Reader reader{data, HEADER_SIZE + META_SIZE};

    std::vector<input> inputs = meta.version == 1 ? parseInputsV1(reader) : parseInputsV2(reader);
    if (reader.bad)
        return Err("TCM file is corrupted or truncated");

    if (inputs.empty())
        return Err("TCM file contains no usable inputs");

    std::sort(inputs.begin(), inputs.end());

    BotReplay replay;
    replay.framerate = meta.tps > 0.f ? meta.tps : 240.f;
    if (meta.hasSeed)
        replay.seed = static_cast<uintptr_t>(meta.seed);
    replay.inputs = std::move(inputs);

    return Ok(replay);
}
} // namespace tcm_import
