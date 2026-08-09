#include "../trajectory/trajectory.hpp"
#include "../core/bot.hpp"
#include "../ui/layers/record_layer.hpp"

#include <Geode/loader/GameEvent.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/utils/Keyboard.hpp>

// $on_mod(Loaded) {
//     KeyboardInputEvent().listen([](KeyboardInputData& data) {
//         auto& bot = Bot::get();
//         int keyInt = static_cast<int>(data.key);

//         bool isKeyRepeat = (data.action ==
//         KeyboardInputData::Action::Repeat); bool isKeyDown = (data.action ==
//         KeyboardInputData::Action::Press);

//         if (bot.allKeybinds.contains(keyInt) && !isKeyRepeat) {
//             for (size_t i = 0; i < 6; i++) {
//                 if (std::find(bot.keybinds[i].begin(), bot.keybinds[i].end(),
//                 keyInt) != bot.keybinds[i].end()) {
//                     bot.heldButtons[i] = isKeyDown;
//                 }
//             }
//         }

//         return ListenerResult::Propagate;
//     }).leak();
// }

void handleKeybind(std::string_view id, bool down, bool repeat, double time) {
    auto& bot = Bot::get();

    if (!down ||
        (LevelEditorLayer::get() && !bot.mod->getSettingValue<bool>("editor_keybinds")) ||
        bot.mod->getSettingValue<bool>("disable_keybinds")) {
        return;
    }

    if (auto scene = CCScene::get()) {
        if (id == "open_menu_keybind" && bot.layer && scene->getChildByIndex(-1) != bot.layer)
            return;
    }

    if (bot.state != state::recording && bot.mod->getSettingValue<bool>("recording_only_keybinds"))
        return;

    static std::unordered_map<std::string_view, geode::Function<void()>> handlers = [] {
        std::unordered_map<std::string_view, geode::Function<void()>> map;

        map["open_menu_keybind"] = []() {
            auto& bot = Bot::get();
            if (bot.layer) {
                static_cast<RecordLayer*>(bot.layer)->onClose(nullptr);
            } else {
                static_cast<RecordLayer*>(bot.layer)->openMenu(bot.mod->getSettingValue<bool>("open_menu_instant"));
            }
        };

        map["toggle_recording_keybind"] = []() {
            Bot::toggleRecording();
        };
        map["toggle_playing_keybind"] = []() {
            Bot::togglePlaying();
        };

        map["toggle_frame_stepper_keybind"] = []() {
            if (PlayLayer::get())
                Bot::toggleFrameStepper();
        };

        map["step_frame_keybind"] = []() {
            if (auto* dispatcher = CCKeyboardDispatcher::get(); dispatcher->getShiftKeyPressed())
                return;
            Bot::frameStep();
        };

        map["backstep_frame_keybind"] = []() {
            Bot::backstepFrame();
        };

        map["toggle_speedhack_keybind"] = []() {
            Bot::toggleSpeedhack();
        };

        map["show_trajectory_keybind"] = []() {
            auto& bot = Bot::get();
            bool newState = !bot.mod->getSavedValue<bool>("macro_show_trajectory");
            bot.mod->setSavedValue("macro_show_trajectory", newState);
            bot.showTrajectory = newState;

            if (auto* layer = static_cast<RecordLayer*>(bot.layer)) {
                if (layer->trajectoryToggle)
                    layer->trajectoryToggle->toggle(newState);
            }
            if (!newState)
                ShowTrajectory::clearTrajectory();
        };

        #ifndef GEODE_IS_IOS
        map["toggle_render_keybind"] = []() {
            if (PlayLayer::get()) {
                auto& bot = Bot::get();
                if (Renderer::toggle()) {
                    Notification::create("Started Rendering", NotificationIcon::Info)->show();
                }
                if (auto* layer = static_cast<RecordLayer*>(bot.layer)) {
                    if (layer->renderToggle)
                        layer->renderToggle->toggle(bot.renderer.recording);
                }
            }
        };
        #endif

        map["toggle_noclip_keybind"] = []() {
            auto& bot = Bot::get();
            bool newState = !bot.mod->getSavedValue<bool>("macro_noclip");
            bot.mod->setSavedValue("macro_noclip", newState);

            if (auto* layer = static_cast<RecordLayer*>(bot.layer)) {
                if (layer->noclipToggle)
                    layer->noclipToggle->toggle(newState);
            }
        };

        return map;
    }();

    if (auto it = handlers.find(id); it != handlers.end()) {
        it->second();
    }
}

$execute {
    struct KeybindEntry {
        std::string_view id;
        bool passRepeat;
    };

    constexpr KeybindEntry keybinds[] = {
        {"open_menu_keybind", false},
        {"toggle_recording_keybind", false},
        {"toggle_playing_keybind", false},
        {"toggle_speedhack_keybind", false},
        {"toggle_frame_stepper_keybind", false},
        {"step_frame_keybind", true},
        {"backstep_frame_keybind", true},
        {"show_trajectory_keybind", false},
        #ifndef GEODE_IS_IOS
        {"toggle_render_keybind", false},
        #endif
        {"toggle_noclip_keybind", false},
    };

    for (const auto& entry : keybinds) {
        listenForKeybindSettingPresses(
            std::string(entry.id),
            [id = entry.id,
             passRepeat = entry.passRepeat](Keybind const&, bool down, bool repeat, double time) {
                handleKeybind(id, down, passRepeat ? repeat : false, time);
            });
    }
}
