#pragma once

#include <Geode/Prelude.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/binding/PlayerObject.hpp>

#include <unordered_map>
#include <unordered_set>
#include <vector>

class TrajectoryNode : public cocos2d::CCDrawNode {
public:
    static TrajectoryNode* create() {
        auto* ret = new TrajectoryNode();
        if (ret && ret->init()) {
            ret->autorelease();
            ret->m_bUseArea = false;
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class ShowTrajectory {
public:
    struct PredictionConfig {
        bool applyReplayInputs = true;
        bool draw = true;
        int maxLength = 10'000'000;
    };

    struct PredictionResult {
        cocos2d::CCPoint position = {};
        cocos2d::CCRect hitbox = {};
        cocos2d::CCRect innerHitbox = {};
        float rotation = 0.f;
        bool player1 = true;
        bool holding = false;
        int score = 0;
    };

    enum Mode : int {
        Hold = 1 << 0,
        Swift = 1 << 1,
        Release = 1 << 2,
        Left = 1 << 3,
        Right = 1 << 4,
    };

    struct BranchResult {
        PredictionResult prediction;
        bool stopped = false;
        bool hasHitbox = false;
    };

    static ShowTrajectory& get() {
        static ShowTrajectory instance;
        return instance;
    }

    void updateMergedColor();
    void setColor1(cocos2d::ccColor3B color);
    void setColor2(cocos2d::ccColor3B color);

    static void clearTrajectory();
    static cocos2d::CCDrawNode* trajectoryNode(bool create = true);
    static void initForLayer(PlayLayer* pl);
    static void uninit();
    static void updateTrajectory(PlayLayer* pl);

    static PredictionResult simulate(
        PlayLayer* pl,
        bool player1,
        int mode,
        bool simulateBothPlayers,
        PredictionConfig config
    );

    static PlayerObject* fakePlayer(bool player1);
    static PlayerObject* otherFakePlayer(PlayerObject* player);
    static bool isFakePlayer(PlayerObject* player);
    static void markFakePlayerDead(PlayerObject* player);
    static bool fakePlayerDead(PlayerObject* player);

    static PlayerObject* ensureFakePlayer(PlayLayer* pl, bool player1);
    static void hideFakePlayer(PlayerObject* player);

    static bool hasActivated(PlayerObject* player, EnhancedGameObject* object);
    static bool realPlayerHasActivated(PlayerObject* player, EnhancedGameObject* object);
    static void rememberActivated(PlayerObject* player, EnhancedGameObject* object);
    static void snapshotObject(GameObject* object);
    static void restoreSnapshots();

    static std::vector<cocos2d::CCPoint> getVertices(
        PlayerObject* player,
        cocos2d::CCRect rect,
        float rotation
    );

    static void drawPlayerHitbox(
        PlayerObject* player,
        cocos2d::CCDrawNode* drawNode,
        cocos2d::ccColor4F outer,
        cocos2d::ccColor4F inner
    );

    static cocos2d::ccColor3B ccc3BFromccc4F(cocos2d::ccColor4F color) {
        return cocos2d::ccc3(
            static_cast<int>(color.r * 255.f),
            static_cast<int>(color.g * 255.f),
            static_cast<int>(color.b * 255.f)
        );
    }

    bool creatingTrajectory = false;
    bool cancelTrajectory = false;

    int length = 312;
    float width = 0.5f;

    cocos2d::ccColor4F color1;
    cocos2d::ccColor4F color2;
    cocos2d::ccColor4F color3;

    TrajectoryNode* m_node = nullptr;

    struct ObjSnapshot {
        GameObject* obj = nullptr;
        RingObject* ring = nullptr;
        EffectGameObject* effect = nullptr;
        bool isActivated = false;
        bool activatedByP1 = false;
        bool activatedByP2 = false;
        bool claimTouch = false;
        bool isDisabled = false;
        bool isDisabled2 = false;
        GLubyte opacity = 255;
        bool visible = true;
    };

    PlayerObject* m_fakePlayer1 = nullptr;
    PlayerObject* m_fakePlayer2 = nullptr;

    bool m_deadP1 = false;
    bool m_deadP2 = false;
    float m_deathRotation = 0.f;

    std::vector<ObjSnapshot> m_objSnapshot;
    std::unordered_set<uintptr_t> m_snapshotObjects;
    std::unordered_map<uintptr_t, bool> m_activatedObjectsP1;
    std::unordered_map<uintptr_t, bool> m_activatedObjectsP2;

};
