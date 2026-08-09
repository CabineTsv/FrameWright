// game_ui.hpp

#pragma once

#include <Geode/Geode.hpp>
#include <Geode/Prelude.hpp>
#include <Geode/ui/Button.hpp>

#include "button_edit_layer.hpp"
#include "../layers/record_layer.hpp"

using namespace geode::prelude;

class Interface {
  public:
    CCLabelBMFont* frameLabel = nullptr;
    CCLabelBMFont* stateLabel = nullptr;

    CCMenu* buttonMenu = nullptr;

    Button* stepFrameBtn = nullptr;
    Button* backstepFrameBtn = nullptr;
    Button* disableStepperBtn = nullptr;
    Button* speedhackBtn = nullptr;

    static Interface& get() {
        static Interface instance;
        return instance;
    }

    static void openButtonEditor() {
        auto* layer = ButtonEditLayer::create();

        layer->m_noElasticity = true;

        layer->show();
    }

    static void addLabels(PlayLayer* pl);

    static void addButtons(PlayLayer* pl);

    static void updateLabels();

    static void updateButtons();
};
