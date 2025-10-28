#pragma once
#include "cocos2d.h"

USING_NS_CC;

enum class EnemyState {
    Walk,
    Dead
};

class Enemy : public Sprite {
public:
    static Enemy* createWithSpriteFrameName(const std::string& frameName);

    virtual bool initWithSpriteFrameName(const std::string& frameName);

    void update(float dt);
    void setState(EnemyState newState);
    EnemyState getState() const { return _state; }

    void playWalkAnimation();
    void playDeadAnimation();

    void setVelocity(const Vec2& v);
    Vec2 getVelocity() const;

    void setPatrolRange(float distance);

private:
    EnemyState _state;
    Vec2 _velocity;
    Action* _currentAction;

    float _leftLimit;
    float _rightLimit;
    float _moveSpeed;
    bool _movingLeft;
    float _spawnX;
};
