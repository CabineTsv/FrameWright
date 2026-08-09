#include "../../core/bot.hpp"
#include "../../utils/utils.hpp"

class AutoclickerLayer : public geode::Popup, public TextInputDelegate {

  private:
    TextInput* holdInput = nullptr;
    TextInput* releaseInput = nullptr;

    CCLabelBMFont* holdLbl = nullptr;
    CCLabelBMFont* releaseLbl = nullptr;
    CCLabelBMFont* holdTitle = nullptr;
    CCLabelBMFont* releaseTitle = nullptr;

    TextInput* holdInput2 = nullptr;
    TextInput* releaseInput2 = nullptr;

    CCLabelBMFont* holdLbl2 = nullptr;
    CCLabelBMFont* releaseLbl2 = nullptr;
    CCLabelBMFont* holdTitle2 = nullptr;
    CCLabelBMFont* releaseTitle2 = nullptr;

    CCMenuItemToggler* toggleP1 = nullptr;
    CCMenuItemToggler* toggleP2 = nullptr;

    void textChanged(CCTextInputNode*) override {
        auto& bot = Bot::get();
        bot.holdFor = geode::utils::numFromString<int>(holdInput->getString()).unwrapOr(0);
        bot.releaseFor = geode::utils::numFromString<int>(releaseInput->getString()).unwrapOr(0);
        bot.holdFor2 = geode::utils::numFromString<int>(holdInput2->getString()).unwrapOr(0);
        bot.releaseFor2 = geode::utils::numFromString<int>(releaseInput2->getString()).unwrapOr(0);

        bot.mod->setSavedValue("autoclicker_hold_for", bot.holdFor);
        bot.mod->setSavedValue("autoclicker_release_for", bot.releaseFor);
        bot.mod->setSavedValue("autoclicker_hold_for2", bot.holdFor2);
        bot.mod->setSavedValue("autoclicker_release_for2", bot.releaseFor2);

        float dt = 1.f / Bot::getTPS();

        holdLbl->setString(fmt::format("{:.3f}s", dt * bot.holdFor).c_str());
        releaseLbl->setString(fmt::format("{:.3f}s", dt * bot.releaseFor).c_str());
        holdLbl2->setString(fmt::format("{:.3f}s", dt * bot.holdFor2).c_str());
        releaseLbl2->setString(fmt::format("{:.3f}s", dt * bot.releaseFor2).c_str());
    }

    bool init() override {
        if (!Popup::init(250, 240, Utils::getTexture().c_str()))
            return false;
        setTitle("Autoclicker");
        m_title->setScale(0.625f);
        m_title->setPositionY(224);

        float x = m_size.width - 72;

        CCLabelBMFont* lbl = CCLabelBMFont::create("Player 1", "goldFont.fnt");
        lbl->setPosition({m_size.width / 2, 195});
        lbl->setScale(0.46f);
        m_mainLayer->addChild(lbl);

        holdTitle = CCLabelBMFont::create("Hold For", "bigFont.fnt");
        holdTitle->setPosition({72, 175});
        holdTitle->setScale(0.37f);
        m_mainLayer->addChild(holdTitle);

        releaseTitle = CCLabelBMFont::create("Release For", "bigFont.fnt");
        releaseTitle->setPosition({x, 175});
        releaseTitle->setScale(0.37f);
        m_mainLayer->addChild(releaseTitle);

        holdLbl = CCLabelBMFont::create("0.325s", "chatFont.fnt");
        holdLbl->setPosition({72, 129});
        holdLbl->setScale(0.4f);
        holdLbl->setOpacity(86);
        m_mainLayer->addChild(holdLbl);

        releaseLbl = CCLabelBMFont::create("0.6s", "chatFont.fnt");
        releaseLbl->setPosition({x, 129});
        releaseLbl->setScale(0.4f);
        releaseLbl->setOpacity(86);
        m_mainLayer->addChild(releaseLbl);

        holdInput = TextInput::create(50, "Frames", "chatFont.fnt");
        holdInput->setPosition({72, 150});
        holdInput->setString(
            geode::utils::numToString(Mod::get()->getSavedValue<int64_t>("autoclicker_hold_for"))
                .c_str());
        holdInput->getInputNode()->setDelegate(this);
        holdInput->getInputNode()->setAllowedChars("0123456789");
        holdInput->getInputNode()->setMaxLabelLength(4);
        m_mainLayer->addChild(holdInput);

        releaseInput = TextInput::create(50, "Frames", "chatFont.fnt");
        releaseInput->setPosition({x, 150});
        releaseInput->setString(
            geode::utils::numToString(Mod::get()->getSavedValue<int64_t>("autoclicker_release_for"))
                .c_str());
        releaseInput->getInputNode()->setDelegate(this);
        releaseInput->getInputNode()->setAllowedChars("0123456789");
        releaseInput->getInputNode()->setMaxLabelLength(4);
        m_mainLayer->addChild(releaseInput);

        // ------------------------

        lbl = CCLabelBMFont::create("Player 2", "goldFont.fnt");
        lbl->setPosition({m_size.width / 2, 115});
        lbl->setScale(0.46f);
        m_mainLayer->addChild(lbl);

        holdTitle2 = CCLabelBMFont::create("Hold For", "bigFont.fnt");
        holdTitle2->setPosition({72, 95});
        holdTitle2->setScale(0.37f);
        m_mainLayer->addChild(holdTitle2);

        releaseTitle2 = CCLabelBMFont::create("Release For", "bigFont.fnt");
        releaseTitle2->setPosition({x, 95});
        releaseTitle2->setScale(0.37f);
        m_mainLayer->addChild(releaseTitle2);

        holdLbl2 = CCLabelBMFont::create("0.325s", "chatFont.fnt");
        holdLbl2->setPosition({72, 49});
        holdLbl2->setScale(0.4f);
        holdLbl2->setOpacity(86);
        m_mainLayer->addChild(holdLbl2);

        releaseLbl2 = CCLabelBMFont::create("0.6s", "chatFont.fnt");
        releaseLbl2->setPosition({x, 49});
        releaseLbl2->setScale(0.4f);
        releaseLbl2->setOpacity(86);
        m_mainLayer->addChild(releaseLbl2);

        holdInput2 = TextInput::create(50, "Frames", "chatFont.fnt");
        holdInput2->setPosition({72, 70});
        holdInput2->setString(
            geode::utils::numToString(Mod::get()->getSavedValue<int64_t>("autoclicker_hold_for2"))
                .c_str());
        holdInput2->getInputNode()->setDelegate(this);
        holdInput2->getInputNode()->setAllowedChars("0123456789");
        holdInput2->getInputNode()->setMaxLabelLength(4);
        m_mainLayer->addChild(holdInput2);

        releaseInput2 = TextInput::create(50, "Frames", "chatFont.fnt");
        releaseInput2->setPosition({x, 70});
        releaseInput2->setString(geode::utils::numToString(
                                     Mod::get()->getSavedValue<int64_t>("autoclicker_release_for2"))
                                     .c_str());
        releaseInput2->getInputNode()->setDelegate(this);
        releaseInput2->getInputNode()->setAllowedChars("0123456789");
        releaseInput2->getInputNode()->setMaxLabelLength(4);
        m_mainLayer->addChild(releaseInput2);

        toggleP1 =
            CCMenuItemExt::createTogglerWithStandardSprites(0.5f, [this](CCMenuItemToggler* sender) {
                AutoclickerLayer::onToggle(sender);
            });
        toggleP1->setPosition({162, 195});
        toggleP1->toggle(Bot::get().autoclickerP1);
        m_buttonMenu->addChild(toggleP1);

        toggleP2 =
            CCMenuItemExt::createTogglerWithStandardSprites(0.5f, [this](CCMenuItemToggler* sender) {
                AutoclickerLayer::onToggle(sender);
            });
        toggleP2->setPosition({162, 115});
        toggleP2->toggle(Bot::get().autoclickerP2);
        m_buttonMenu->addChild(toggleP2);

        ButtonSprite* btnSpr = ButtonSprite::create("OK");
        btnSpr->setScale(0.7f);
        CCMenuItemSpriteExtra* btn =
            CCMenuItemExt::createSpriteExtra(btnSpr, [this](CCMenuItemSpriteExtra* sender) {
                AutoclickerLayer::onClose(sender);
            });
        btn->setPosition({m_size.width / 2, 24});
        m_buttonMenu->addChild(btn);

        textChanged(nullptr);
        updateInputs();

        return true;
    }

    void onToggle(CCObject*) {
        Loader::get()->queueInMainThread([this] {
            auto& bot = Bot::get();
            bot.autoclickerP1 = toggleP1->isToggled();
            bot.autoclickerP2 = toggleP2->isToggled();
            bot.mod->setSavedValue("autoclicker_p1", bot.autoclickerP1);
            bot.mod->setSavedValue("autoclicker_p2", bot.autoclickerP2);
            updateInputs();
        });
    }

    void updateInputs() {
        auto& bot = Bot::get();

        holdTitle->setOpacity(bot.autoclickerP1 ? 255 : 100);
        holdLbl->setOpacity(bot.autoclickerP1 ? 86 : 23);
        Utils::setTextInputEnabled(holdInput, bot.autoclickerP1);

        holdTitle2->setOpacity(bot.autoclickerP2 ? 255 : 100);
        holdLbl2->setOpacity(bot.autoclickerP2 ? 86 : 23);
        Utils::setTextInputEnabled(holdInput2, bot.autoclickerP2);

        releaseTitle->setOpacity(bot.autoclickerP1 ? 255 : 100);
        releaseLbl->setOpacity(bot.autoclickerP1 ? 86 : 23);
        Utils::setTextInputEnabled(releaseInput, bot.autoclickerP1);

        releaseTitle2->setOpacity(bot.autoclickerP2 ? 255 : 100);
        releaseLbl2->setOpacity(bot.autoclickerP2 ? 86 : 23);
        Utils::setTextInputEnabled(releaseInput2, bot.autoclickerP2);
    }

  public:
    static AutoclickerLayer* create() {
        auto* layer = new AutoclickerLayer();
        if (layer && layer->init()) {
            layer->autorelease();
            return layer;
        }
        delete layer;
        return nullptr;
    }

};
