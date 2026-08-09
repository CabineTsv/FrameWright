#include "../core/bot.hpp"

#include <Geode/modify/PlayLayer.hpp>

#include <algorithm>

namespace {
int rollAttemptsShowcaseTarget() {
    auto* mod = Mod::get();

    int64_t min = mod->getSavedValue<int64_t>("attempts_showcase_min");
    int64_t max = mod->getSavedValue<int64_t>("attempts_showcase_max");

    min = std::clamp<int64_t>(min, 0, 100);
    max = std::clamp<int64_t>(max, 0, 100);

    if (min > max)
        std::swap(min, max);

    return geode::utils::random::generate(static_cast<int>(min), static_cast<int>(max));
}
} // namespace

class $modify(PlayLayer) {

    void resetLevel() {
        PlayLayer::resetLevel();

        auto& bot = Bot::get();
        bot.attemptsShowcaseTarget = -1;
        bot.attemptsShowcaseHolding = false;
        bot.attemptsShowcaseHeld = false;

        if (bot.state != state::playing)
            return;

        if (!bot.mod->getSavedValue<bool>("macro_attempts_showcase"))
            return;

        bot.attemptsShowcaseTarget = rollAttemptsShowcaseTarget();
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        auto& bot = Bot::get();

        if (bot.state != state::playing)
            return;

        if (bot.attemptsShowcaseTarget < 0)
            return;

        if (!bot.mod->getSavedValue<bool>("macro_attempts_showcase"))
            return;

        if (!m_player1 || m_player1->m_isDead)
            return;

        if (m_isPaused || m_levelEndAnimationStarted || m_hasCompletedLevel)
            return;

        if (getCurrentPercentInt() < bot.attemptsShowcaseTarget)
            return;

        bot.attemptsShowcaseTarget = -1;

        if (bot.mod->getSavedValue<bool>("attempts_showcase_death_contact")) {
            bot.attemptsShowcaseHolding = true;
            return;
        }

        destroyPlayer(m_player1, nullptr);
    }
};
