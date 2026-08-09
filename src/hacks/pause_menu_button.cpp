#include "../ui/layers/record_layer.hpp"

#include <Geode/modify/PauseLayer.hpp>

class $modify(xdBotPauseLayerButton, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

#ifdef GEODE_IS_DESKTOP
        if (!Mod::get()->getSavedValue<bool>("menu_show_button"))
            return;
#endif

        auto* sprite = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
        sprite->setScale(0.35f);

        auto* btn = CCMenuItemExt::createSpriteExtra(sprite, [this](CCMenuItemSpriteExtra* sender) {
            static_cast<RecordLayer*>(Bot::get().layer)->openMenu2(sender);
        });

        if (!Loader::get()->isModLoaded("geode.node-ids")) {
            auto* menu = CCMenu::create();
            menu->setID("button"_spr);
            addChild(menu);
            btn->setPosition({214, 88});
            menu->addChild(btn);
            return;
        }

        CCNode* menu = this->getChildByID("right-button-menu");
        menu->addChild(btn);
        menu->updateLayout();
    }
};
