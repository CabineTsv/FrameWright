#pragma once

#include <Geode/Geode.hpp>
#include <Geode/Prelude.hpp>
#include <Geode/ui/SliderNode.hpp>

#include <map>
#include <string>
#include <vector>

using namespace geode::prelude;

const std::vector<std::string> indexToID = {
    "button_off", "button_advance_frame", "button_backstep_frame",
    "button_speedhack"};

const std::map<std::string, int> IDtoIndex{
    {"button_off", 0},
    {"button_advance_frame", 1},
    {"button_backstep_frame", 2},
    {"button_speedhack", 3}};

const std::map<std::string, std::string> IDtoName{
    {"button_off", "Frame Stepper Off"},
    {"button_advance_frame", "Advance Frame"},
    {"button_backstep_frame", "Backstep Frame"},
    {"button_speedhack", "Toggle Speedhack"}};

struct MovingButton {
    size_t index = 0;
    CCSprite *sprite = nullptr;
    cocos2d::CCPoint offset = ccp(0, 0);
};

class ButtonEditLayer : public geode::Popup {

  private:
    bool init() override;

  public:
    static ButtonEditLayer* create() {
        auto* layer = new ButtonEditLayer();
        if (layer && layer->init()) {
            layer->autorelease();
            return layer;
        }
        delete layer;
        return nullptr;
    }

    Mod *mod = nullptr;
    CCMenu *menu = nullptr;

    SliderNode *scaleSlider = nullptr;
    SliderNode *opacitySlider = nullptr;

    CCLabelBMFont *scaleLbl = nullptr;
    CCLabelBMFont *opacityLbl = nullptr;
    CCLabelBMFont *selectedLbl = nullptr;

    std::vector<CCSprite *> spriteButtons;

    MovingButton movingButton;

    size_t currentSelected = 0;

    std::map<std::string, cocos2d::CCPoint> positions;
    std::map<std::string, float> scales;
    std::map<std::string, float> opacities;

    bool ccTouchBegan(cocos2d::CCTouch *touch,
                      cocos2d::CCEvent *event) override;
    void ccTouchMoved(cocos2d::CCTouch *touch,
                      cocos2d::CCEvent *event) override;
    void ccTouchEnded(cocos2d::CCTouch *touch,
                      cocos2d::CCEvent *event) override;

    void updateSelectedLabels();

    void updateScale(CCObject*);

    void updateOpacity(CCObject*);

    void updateSelected(std::string selected);

    void addSliders();

    void addSprites();

    static bool isPointInButton(cocos2d::CCPoint clickPos,
                                cocos2d::CCPoint btnPos,
                                cocos2d::CCSize btnSize);
};
