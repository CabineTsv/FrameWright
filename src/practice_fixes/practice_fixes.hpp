// practice_fixes.hpp
#pragma once

#include "../core/bot.hpp"
#include "checkpoint.hpp"
#include <Geode/modify/PlayLayer.hpp>

class PracticeFix {
  public:
    static bool shouldEnable() {
        auto& bot = Bot::get();
        auto* pl = PlayLayer::get();

        if (!pl || !pl->m_isPracticeMode)
            return false;

        return bot.alwaysPracticeFixes || bot.state == state::recording || bot.state == state::playing;
    }

    static void clearStoredFrames();
    static void saveFrameStepperFrame();
    static bool backstepFrame(int frames = 1);
    static bool isLoadingFrameStepperBackstep();
    static bool applyFrameStepperBackstep(CheckpointObject* checkpoint);
};

class PlayerPracticeFixes {
  public:
    struct SavedState {
        SupplementalPlayerState supplemental;
    };

    static SavedState saveData(PlayerObject* p) {
        SavedState s;
        if (!p)
            return s;
        s.supplemental = SupplementalPlayerState(p);
        return s;
    }

    static void applyData(PlayerObject* p, const SavedState& s) {
        if (!p)
            return;
        s.supplemental.apply(p);
    }

    static void transfer(PlayerObject* from, PlayerObject* to, bool applyPos) {
        if (!from || !to)
            return;
        SavedState s = saveData(from);
        if (applyPos) {
            to->setPosition(from->getPosition());
            to->setRotation(from->getRotation());
        }
        applyData(to, s);
    }
};
