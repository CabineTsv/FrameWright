#include "../core/bot.hpp"
#include "clickbot.hpp"

#include <Geode/modify/GJBaseGameLayer.hpp>

$execute { 
    auto & bot = Bot::get();

    if (!bot.mod->setSavedValue("clickbot_defaults5", true)) {
        bot.mod->setSavedValue("clickbot_holding_only", true);
        bot.mod->setSavedValue("clickbot_playing_only", false);
    }

    if (!bot.mod->setSavedValue("clickbot_defaults4", true)) {
        std::filesystem::path dir = bot.mod->getResourcesDir();
        ClickSetting setts;

        for (const auto& str : buttonNames) {
            setts.path = dir / fmt::format("default_{}.mp3", str);
            matjson::Value data = matjson::Serialize<ClickSetting>::to_json(setts);
            bot.mod->setSavedValue(str, data);
        }

        bot.mod->setSavedValue("clickbot_volume", 100);
        bot.mod->setSavedValue("clickbot_pitch", 1.f);
    }

    bot.clickbotEnabled = bot.mod->getSavedValue<bool>("clickbot_enabled");
    bot.clickbotOnlyPlaying = bot.mod->getSavedValue<bool>("clickbot_playing_only");
    bot.clickbotOnlyHolding = bot.mod->getSavedValue<bool>("clickbot_holding_only");

    Clickbot::updateSounds();

};

class $modify(GJBaseGameLayer) {
    
    void handleButton(bool hold, int button, bool player2) {
        GJBaseGameLayer::handleButton(hold, button, player2);
        auto& bot = Bot::get();

        if (!bot.clickbotEnabled) return;
        if (button > 3 || (!hold && bot.clickbotOnlyHolding)) return;

        PlayLayer* pl = PlayLayer::get();

        if (!pl) return;
        if (bot.clickbotOnlyPlaying && bot.state != state::playing) return;

        std::string btn = button == 1 ? "click" : (button == 2 ? "left" : "right");
        std::string id = (hold ? "hold_" : "release_") + btn;
        Clickbot::playSound(id);
    }

};

FMOD::Sound* Clickbot::getSound(std::string id) {
    auto& c = get();

    if (id == "hold_click")
        return c.holdClick;
    else if (id == "release_click")
        return c.releaseClick;
    else if (id == "hold_left")
        return c.holdLeft;
    else if (id == "release_left")
        return c.releaseLeft;
    else if (id == "hold_right")
        return c.holdRight;
    else if (id == "release_right")
        return c.releaseRight;

    return nullptr;
}

void Clickbot::setSound(std::string id, FMOD::Sound* sound) {
    auto& c = get();

    if (id == "hold_click")
        c.holdClick = sound;
    else if (id == "release_click")
        c.releaseClick = sound;
    else if (id == "hold_left")
        c.holdLeft = sound;
    else if (id == "release_left")
        c.releaseLeft = sound;
    else if (id == "hold_right")
        c.holdRight = sound;
    else if (id == "release_right")
        c.releaseRight = sound;
}

void Clickbot::playSound(std::string id) {
    auto& c = get();
    if (!c.system) return updateSounds();

    auto& bot = Bot::get();
    matjson::Value data = bot.mod->getSavedValue<matjson::Value>(id);
    ClickSetting settings = matjson::Serialize<ClickSetting>::from_json(data);

    if (settings.disabled) return;

    FMOD::Sound* sound = getSound(id);

    if (!sound) return;

    int masterVol = bot.mod->getSavedValue<int64_t>("clickbot_volume");
    if (settings.volume == 0 || masterVol == 0) return;

    FMOD_RESULT result;

    result = c.system->playSound(sound, nullptr, false, &c.channel);
    if (result != FMOD_OK) return log::debug("Click sound errored. ID: 2");

    result = c.channel->setVolume((settings.volume / 100.f) * (masterVol / 100.f));
    if (result != FMOD_OK) return log::debug("Click sound errored. ID: 3");

    result = c.channel->setPitch(bot.currentPitch);
    if (result != FMOD_OK) return log::debug("Click sound errored. ID: 4");

    FMOD::DSP* pitchShifter = c.pitchShifter;
    if (!pitchShifter) return updateSounds();

    result = pitchShifter->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, settings.pitch * bot.mod->getSavedValue<float>("clickbot_pitch"));
    if (result != FMOD_OK) return log::debug("Click sound errored. ID: 6");

    result = c.channel->addDSP(0, pitchShifter);
    if (result != FMOD_OK) return log::debug("Click sound errored. ID: 7");
}

void Clickbot::updateSounds() {
    auto& c = get();
    FMOD_RESULT result;

    if (!c.system) {
        FMODAudioEngine* fmod = FMODAudioEngine::sharedEngine();
        c.system = fmod->m_system;
    }

    if (!c.system) return;

    for (std::string name : buttonNames) {
        matjson::Value data = Bot::get().mod->getSavedValue<matjson::Value>(name);
        ClickSetting settings = matjson::Serialize<ClickSetting>::from_json(data);
        if (!std::filesystem::exists(settings.path)) continue;

        FMOD::Sound* sound = getSound(name);
        result = c.system->createSound(geode::utils::string::pathToString(settings.path).c_str(), FMOD_DEFAULT, nullptr, &sound);
        if (result != FMOD_OK) {
            log::debug("Click sound errored. ID: 1");
            continue;
        }

        setSound(name, sound);
    }

    if (!c.pitchShifter) {
        result = c.system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &c.pitchShifter);
        if (result != FMOD_OK) return log::debug("Click sound errored. ID: 5");
    }
}
