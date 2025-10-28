#pragma once
#include "cocos2d.h"

USING_NS_CC;

class Fireball : public Sprite
{
public:
    static Fireball* createFireball(bool facingRight, TMXTiledMap* map);
    void update(float delta);

private:
    bool _facingRight;
    Vec2 _velocity;
    TMXTiledMap* _map;
    int _bounceCount = 0;
};
