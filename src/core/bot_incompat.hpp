#pragma once

namespace bot_incompat {
bool hasIncompatibleMods();

bool enabledIncompatibleGDSettings();

void autoDisableBotSettings();
void restoreAutoDisabledSettings();

// One-time setup: tells Click Between Frames not to use its own bundled
// physics-bypass, since Framewright has its own (TPS Bypass / Lock Delta).
// Running both at once means two separate systems fighting over control of
// physics timing, which is exactly the kind of thing that causes desyncs.
void configureClickBetweenFrames();
} // namespace bot_incompat
