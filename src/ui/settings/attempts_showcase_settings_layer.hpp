#pragma once

#include "../../core/bot.hpp"
#include "../../utils/utils.hpp"

#include <algorithm>

class AttemptsShowcaseSettingsLayer : public geode::Popup, public TextInputDelegate {

  private:
    TextInput* minInput = nullptr;
    TextInput* maxInput = nullptr;

    void textChanged(CCTextInputNode*) override {
        int64_t min = geode::utils::numFromString<int64_t>(minInput->getString()).unwrapOr(0);
        int64_t max = geode::utils::numFromString<int64_t>(maxInput->getString()).unwrapOr(100);

        min = std::clamp<int64_t>(min, 0, 100);
        max = std::clamp<int64_t>(max, 0, 100);

        Mod::get()->setSavedValue("attempts_showcase_min", min);
        Mod::get()->setSavedValue("attempts_showcase_max", max);
    }

    bool init() override {
        if (!Popup::init(220, 210, Utils::getTexture().c_str()))
            return false;
        setTitle("Attempts Showcase");
        m_title->setScale(0.55f);
        m_title->setPositionY(190);

        Utils::setBackgroundColor(m_bgSprite);

        float x1 = m_size.width / 2 - 45;
        float x2 = m_size.width / 2 + 45;

        CCLabelBMFont* lbl = CCLabelBMFont::create("Min %", "bigFont.fnt");
        lbl->setPosition({x1, 142});
        lbl->setScale(0.4f);
        m_mainLayer->addChild(lbl);

        lbl = CCLabelBMFont::create("Max %", "bigFont.fnt");
        lbl->setPosition({x2, 142});
        lbl->setScale(0.4f);
        m_mainLayer->addChild(lbl);

        minInput = TextInput::create(60, "0-100", "chatFont.fnt");
        minInput->setPosition({x1, 118});
        minInput->setString(
            geode::utils::numToString(Mod::get()->getSavedValue<int64_t>("attempts_showcase_min"))
                .c_str());
        minInput->getInputNode()->setDelegate(this);
        minInput->getInputNode()->setAllowedChars("0123456789");
        minInput->getInputNode()->setMaxLabelLength(3);
        m_mainLayer->addChild(minInput);

        maxInput = TextInput::create(60, "0-100", "chatFont.fnt");
        maxInput->setPosition({x2, 118});
        maxInput->setString(
            geode::utils::numToString(Mod::get()->getSavedValue<int64_t>("attempts_showcase_max"))
                .c_str());
        maxInput->getInputNode()->setDelegate(this);
        maxInput->getInputNode()->setAllowedChars("0123456789");
        maxInput->getInputNode()->setMaxLabelLength(3);
        m_mainLayer->addChild(maxInput);

        CCLabelBMFont* deathContactLbl = CCLabelBMFont::create("Death Contact", "bigFont.fnt");
        deathContactLbl->setPosition({x1, 84});
        deathContactLbl->setScale(0.4f);
        m_mainLayer->addChild(deathContactLbl);

        CCMenuItemToggler* deathContactToggle = CCMenuItemExt::createTogglerWithStandardSprites(
            0.7f, [this](CCMenuItemToggler* sender) {
                AttemptsShowcaseSettingsLayer::onToggleDeathContact(sender);
            });
        deathContactToggle->setPosition({x2, 84});
        deathContactToggle->toggle(
            Mod::get()->getSavedValue<bool>("attempts_showcase_death_contact"));
        m_buttonMenu->addChild(deathContactToggle);

        CCLabelBMFont* info = CCLabelBMFont::create(
            "Each attempt, dies at a random %\nin this range. Death Contact holds\n"
            "an input and waits for a natural\ndeath instead of an instant kill.",
            "chatFont.fnt");
        info->setPosition({m_size.width / 2, 52});
        info->setScale(0.32f);
        info->setOpacity(120);
        info->setAlignment(kCCTextAlignmentCenter);
        m_mainLayer->addChild(info);

        ButtonSprite* btnSpr = ButtonSprite::create("OK");
        btnSpr->setScale(0.7f);
        CCMenuItemSpriteExtra* btn =
            CCMenuItemExt::createSpriteExtra(btnSpr, [this](CCMenuItemSpriteExtra* sender) {
                AttemptsShowcaseSettingsLayer::onClose(sender);
            });
        btn->setPosition({m_size.width / 2, 20});
        m_buttonMenu->addChild(btn);

        return true;
    }

    void onToggleDeathContact(CCObject* obj) {
        CCMenuItemToggler* toggle = typeinfo_cast<CCMenuItemToggler*>(obj);
        if (!toggle)
            return;

        Mod::get()->setSavedValue("attempts_showcase_death_contact", !toggle->isToggled());
    }

  public:
    static AttemptsShowcaseSettingsLayer* create() {
        auto* layer = new AttemptsShowcaseSettingsLayer();
        if (layer && layer->init()) {
            layer->autorelease();
            return layer;
        }
        delete layer;
        return nullptr;
    }
};
