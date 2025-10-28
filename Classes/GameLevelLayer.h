#ifndef GameLevelLayer_h
#define GameLevelLayer_h

#include "cocos2d.h"
#include "Player.h"
#include "SimpleAudioEngine.h"
#include "Enemy.h"
#include "QuestionBlock.h"

USING_NS_CC;


struct tileInfo
{
    int gid;
    float x;
    float y;
    Vec2 tilePos;
};

class GameLevelLayer : public Layer
{
    // Fields
    TMXTiledMap *_map;
    TMXLayer *_wallLayer;
    TMXLayer *_hazardLayer;
    Player *_player;
    bool _isGameOver;
    std::vector<Enemy*> _enemies;
    std::vector<QuestionBlock*> _questionBlocks;
    
    // Schedule update handler
    void update(float dt);
    
    // Collision
    Vec2 tileCoordForPosition(Vec2 position);
    Rect tileRectFromTileCoords(Vec2 tileCoords);
    std::vector<tileInfo> getSurroundingTilesAtPosition(Vec2 position, TMXLayer* layer);
    void handleHazardCollisions(Player* player);
    void checkForAndResolveCollisions(Player *player);
    void checkPlayerEnemyCollisions();
    void checkPlayerItemCollisions();
    void checkFireballEnemyCollisions();
    
    // Camera on screen
    void setViewpointCenter(Vec2 position);
    
    // Menu
    void gameOver(bool playerDidWin);
    
    // Menu Callback
    void replayButtonCallback(Ref* pSender);
    
    // Helper
    void checkForWin();
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    // implement the "static create()" method manually
    CREATE_FUNC(GameLevelLayer);
    
    void initKeyboardListener();
    void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
};

#endif /* GameLevelLayer_h */
