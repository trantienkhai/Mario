#pragma once
#include "cocos2d.h"

USING_NS_CC;

enum class BossState {
    Idle,
    Dead
};

class Boss : public Sprite {
public:
    static Boss* createBoss();

    virtual bool initBoss();

    void update(float dt);
    void setState(BossState newState);
    BossState getState() const { return _state; }

    void playIdleAnimation();
    void playDeadAnimation();

    void setPatrolRange(float distance);
    void setVelocity(const Vec2& v);
    Vec2 getVelocity() const;

private:
    BossState _state;
    Action* _currentAction;
    Vec2 _velocity;
    float _leftLimit;
    float _rightLimit;
    float _moveSpeed;
    bool _movingLeft;
    float _spawnX;
};

