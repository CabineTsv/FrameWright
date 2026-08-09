#include "../core/bot.hpp"
#include "../practice_fixes/practice_fixes.hpp"

#include <Geode/modify/CCParticleSystem.hpp>
#include <Geode/modify/PlayLayer.hpp>

class $modify(FrameStepperPlayLayer, PlayLayer) {
    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        if (Bot::get().frameStepper)
            Bot::toggleFrameStepper();
    }

    void resetLevel() {
        if (!PracticeFix::isLoadingFrameStepperBackstep())
            PracticeFix::clearStoredFrames();
        PlayLayer::resetLevel();
        if (Bot::get().frameStepper && !PracticeFix::isLoadingFrameStepperBackstep())
            PracticeFix::saveFrameStepperFrame();
    }

    void onQuit() {
        PracticeFix::clearStoredFrames();
        PlayLayer::onQuit();
    }
};

class $modify(FrameStepperParticles, CCParticleSystem) {
    void update(float dt) override {
        auto& bot = Bot::get();
        if (!PlayLayer::get() || !bot.frameStepper)
            return CCParticleSystem::update(dt);

        if (bot.stepFrameParticle > 0) {
            bot.stepFrameParticle--;
            return CCParticleSystem::update(dt);
        }
    }
};
