#include "Fireball.h"
#include "SimpleAudioEngine.h"
#include "GameLevelLayer.h"

using namespace CocosDenshion;

Fireball* Fireball::createFireball(bool facingRight, TMXTiledMap* map)
{
    auto fireball = new Fireball();
    if (fireball && fireball->initWithSpriteFrameName("fire-ball-1"))
    {
        fireball->_facingRight = facingRight;
        fireball->_velocity = Vec2(facingRight ? 200.0f : -200.0f, 0);
        fireball->_map = map;
        fireball->autorelease();
        fireball->setName("fireball");

        Vector<SpriteFrame*> frames;
        for (int i = 1; i <= 4; ++i)
        {
            auto f = SpriteFrameCache::getInstance()->getSpriteFrameByName(
                StringUtils::format("fire-ball-%d", i));
            if (f) frames.pushBack(f);
        }
        if (!frames.empty())
        {
            auto anim = RepeatForever::create(Animate::create(
                Animation::createWithSpriteFrames(frames, 0.08f)));
            fireball->runAction(anim);
        }

        fireball->scheduleUpdate();
        SimpleAudioEngine::getInstance()->playEffect("music/fireball.mp3");
        return fireball;
    }

    CC_SAFE_DELETE(fireball);
    return nullptr;
}


void Fireball::update(float delta)
{
    _velocity.y -= 900.0f * delta;
    setPosition(getPosition() + _velocity * delta);

    auto wallLayer = _map->getLayer("walls");
    if (!wallLayer) return;

    auto tileSize = _map->getTileSize();
    auto mapSize = _map->getMapSize();
    Rect fireRect = getBoundingBox();

    bool hitGround = false;

    float checkWidth = tileSize.width * 0.5f;
    float checkHeight = tileSize.height * 0.2f;
    Vec2 checkPos = getPosition() - Vec2(0, checkHeight * 2);

    for (int dx = -1; dx <= 1; ++dx)
    {
        Vec2 testPos = checkPos + Vec2(dx * checkWidth, 0);
        int tileX = testPos.x / tileSize.width;
        int tileY = (mapSize.height * tileSize.height - testPos.y) / tileSize.height;

        if (tileX < 0 || tileY < 0 || tileX >= mapSize.width || tileY >= mapSize.height)
            continue;

        int gid = wallLayer->getTileGIDAt(Vec2(tileX, tileY));
        if (gid == 0) continue;

        Rect tileRect(tileX * tileSize.width,
            mapSize.height * tileSize.height - (tileY + 1) * tileSize.height,
            tileSize.width, tileSize.height);

        if (fireRect.intersectsRect(tileRect))
        {
            hitGround = true;

            if (_velocity.y < 0)
            {
                // Đặt lại lên mặt đất
                setPositionY(tileRect.getMaxY() + fireRect.size.height * 0.5f);
                _velocity.y = 250.0f;
                _bounceCount++;

                if (_bounceCount > 4)
                {
                    removeFromParent();
                    return;
                }
            }
            else
            {
                removeFromParent();
                return;
            }
            break;
        }
    }

    float mapWidth = mapSize.width * tileSize.width;
    if (getPositionX() < 0 || getPositionX() > mapWidth)
    {
        removeFromParent();
        return;
    }
}