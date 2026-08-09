#pragma once

#include <Geode/Geode.hpp>
#include <Geode/Prelude.hpp>
#include <Geode/ui/SliderNode.hpp>

#include <optional>

#include "../../hacks/clickbot.hpp"
#include "record_layer.hpp"

using namespace geode::prelude;

const std::unordered_map<int, std::string> buttons = {{1, ""}};

class ClickSettingsLayer : public geode::Popup {

  private:
    ClickSetting settings;
    std::string button;

    SliderNode *volumeSlider = nullptr;
    SliderNode *pitchSlider = nullptr;

    CCLabelBMFont *filenameLabel = nullptr;
    CCLabelBMFont *volumeLabel = nullptr;
    CCLabelBMFont *pitchLabel = nullptr;

    CCMenuItemToggler *disableToggle = nullptr;
    async::TaskHolder<Result<std::optional<std::filesystem::path>>>
        m_importTask;

    bool init(std::string button, geode::Popup *layer);

  public:
    geode::Popup *clickbotLayer = nullptr;

    static ClickSettingsLayer *create(std::string button, geode::Popup *layer);

    void saveSettings() {
        matjson::Value data =
            matjson::Serialize<ClickSetting>::to_json(settings);
        Mod::get()->setSavedValue(button, data);

        Clickbot::updateSounds();
    }

    void onSelectFile(CCObject *);
};

class ClickbotLayer : public geode::Popup {

    SliderNode *volumeSlider = nullptr;
    SliderNode *pitchSlider = nullptr;

    CCLabelBMFont *volumeLabel = nullptr;
    CCLabelBMFont *pitchLabel = nullptr;

  private:
    bool init() override;

  public:
    static ClickbotLayer* create() {
        auto* layer = new ClickbotLayer();
        if (layer && layer->init()) {
            layer->autorelease();
            return layer;
        }
        delete layer;
        return nullptr;
    }

    std::vector<CCLabelBMFont *> labels;

    void openClickSettings(CCObject *obj) {
        std::string id = static_cast<CCMenuItemSpriteExtra *>(obj)->getID();
        ClickSettingsLayer::create(id, static_cast<geode::Popup *>(this))
            ->show();
    }

    void updateLabels();
};
