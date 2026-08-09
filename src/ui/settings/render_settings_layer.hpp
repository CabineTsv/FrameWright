#pragma once

#include <Geode/Geode.hpp>
#include <Geode/Prelude.hpp>
#include <Geode/ui/SliderNode.hpp>

using namespace geode::prelude;

class RenderSettingsLayer : public geode::Popup, public TextInputDelegate {

  public:
    SliderNode* sfxSlider = nullptr;
    SliderNode* musicSlider = nullptr;
    TextInput *fadeInInput = nullptr;
    TextInput *fadeOutInput = nullptr;
    TextInput *extensionInput = nullptr;

    TextInput *argsInput = nullptr;
    TextInput *audioArgsInput = nullptr;
    TextInput *secondsInput = nullptr;
    TextInput *videoArgsInput = nullptr;

    Mod *mod = nullptr;

  private:
    bool init() override;

  public:
    ~RenderSettingsLayer() override {
        if (argsInput) {
            argsInput->defocus();
            argsInput->setDelegate(nullptr);
        }
        if (audioArgsInput) {
            audioArgsInput->defocus();
            audioArgsInput->setDelegate(nullptr);
        }
        if (secondsInput) {
            secondsInput->defocus();
            secondsInput->setDelegate(nullptr);
        }
        if (videoArgsInput) {
            videoArgsInput->defocus();
            videoArgsInput->setDelegate(nullptr);
        }
    }

    static RenderSettingsLayer* create() {
        auto* layer = new RenderSettingsLayer();
        if (layer && layer->init()) {
            layer->autorelease();
            return layer;
        }
        delete layer;
        return nullptr;
    }

    void close(CCObject *) { keyBackClicked(); }

    void textChanged(CCTextInputNode *node) override;
    void onSlider(CCObject *);
    void onDefaults(CCObject *);
    void showInfoPopup(CCObject *);
};
