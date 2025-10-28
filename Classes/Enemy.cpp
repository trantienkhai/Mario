#include "Enemy.h"

USING_NS_CC;

Enemy* Enemy::createWithSpriteFrameName(const std::string& frameName) {
    Enemy* enemy = new Enemy();
    if (enemy && enemy->initWithSpriteFrameName(frameName)) {
        enemy->autorelease();
        return enemy;
    }
    CC_SAFE_DELETE(enemy);
    return nullptr;
}

bool Enemy::initWithSpriteFrameName(const std::string& frameName) {
    if (!Sprite::initWithSpriteFrameName(frameName))
        return false;

    _state = EnemyState::Walk;
    _moveSpeed = 20.0f;
    _velocity = Vec2(-_moveSpeed, 0.0f);
    _currentAction = nullptr;

    _spawnX = 0;
    _leftLimit = 0;
    _rightLimit = 0;
    _movingLeft = true;

    playWalkAnimation();
    scheduleUpdate();

    return true;
}

void Enemy::setVelocity(const Vec2& v) {
    _velocity = v;
}
Vec2 Enemy::getVelocity() const {
    return _velocity;
}

void Enemy::setState(EnemyState newState) {
    if (_state == newState)
        return;
    _state = newState;

    if (_currentAction) {
        stopAction(_currentAction);
        CC_SAFE_RELEASE_NULL(_currentAction);
    }

    switch (_state) {
    case EnemyState::Walk:
        playWalkAnimation();
        break;
    case EnemyState::Dead:
        playDeadAnimation();
        break;
    }
}

void Enemy::playWalkAnimation() {
    Vector<SpriteFrame*> frames;
    for (int i = 1; i <= 2; i++) {
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(
            StringUtils::format("castle-gumba-%d", i));
        if (frame)
            frames.pushBack(frame);
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.2f);
    auto animate = Animate::create(animation);
    _currentAction = RepeatForever::create(animate);
    _currentAction->retain();
    runAction(_currentAction);
}

void Enemy::setPatrolRange(float distance)
{
    _spawnX = getPositionX();
    _leftLimit = _spawnX - distance;
    _rightLimit = _spawnX + distance;
}

void Enemy::playDeadAnimation() {
    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("castle-gumba-3");
    if (frame)
        setSpriteFrame(frame);

    runAction(Sequence::create(
        ScaleTo::create(0.15f, 1.0f, 0.3f),
        FadeOut::create(0.25f),
        RemoveSelf::create(),
        nullptr
    ));
}

void Enemy::update(float dt) {
    if (_state == EnemyState::Dead)
        return;

    Vec2 pos = getPosition();

    if (_movingLeft)
    {
        pos.x -= _moveSpeed * dt;
        if (pos.x <= _leftLimit)
        {
            _movingLeft = false;
            setFlippedX(true); 
        }
    }
    else
    {
        pos.x += _moveSpeed * dt;
        if (pos.x >= _rightLimit)
        {
            _movingLeft = true;
            setFlippedX(false); 
        }
    }

    setPosition(pos);

}
