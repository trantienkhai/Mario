#ifndef Player_h
#define Player_h
#include "cocos2d.h"
#include <string>
#include "SimpleAudioEngine.h"

USING_NS_CC;

enum class PlayerState {
    Idle,
    Run,
    Jump
};

enum class PlayerForm {
    Normal,
    Fire
};

class Player : public Sprite
{
    // Default Data/Function Members of C++ class is private
    // Note: struct is versus: default is public
    Vec2 _velocity;
    Vec2 _desiredPosition;
    bool _isOnGround;
    bool _isJumping;
    bool _isMoving;
    float _moveDirection;
    bool _canJump;
    bool _isInvincible;
    PlayerState _state;
    cocos2d::Action* _currentAction;
    PlayerForm _form = PlayerForm::Normal;
    bool _canShoot = false;

    void playIdleAnimation();
    void playRunAnimation();
    void playJumpAnimation();
public:
    // Accessors
    void setDesiredPosition(const Vec2&);
    Vec2 getDesiredPosition();
    void setVelocity(const Vec2&);
    Vec2 getVelocity();
    void setOnGroundFlag(const bool);
    bool getOnGroundFlag();
    void setIsMovingFlag(const bool);
    bool getIsMovingFlag();
    void setIsJumpingFlag(const bool);
    bool getIsJumpingFlag();
    void setMoveDirection(float value);
    float getMoveDirection() const;
    bool isInvincible() const { return _isInvincible; }
    void setInvincible(bool v) { _isInvincible = v; }
    void setForm(PlayerForm form);
    PlayerForm getForm() const { return _form; }
    bool canShoot() const { return _canShoot; }
    //void shootFireball();


    // Create Sprite
    static Player* createWithSpriteFrameName(const std::string& frameName);
    bool initWithSpriteFrameName(const std::string& frameName);
    
    // Scheduled methods
    void update(const float dt) override;
    
    // Get bounding box of player
    Rect getCollisionBoundingBox();
    

    CREATE_FUNC(Player);
    void setState(PlayerState newState);
    PlayerState getState() const { return _state; }
};

#endif /* Player_h */
