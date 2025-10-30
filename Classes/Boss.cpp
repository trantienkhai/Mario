#include "Boss.h"

USING_NS_CC;

Boss* Boss::createBoss() {
    Boss* boss = new Boss();
    if (boss && boss->initBoss()) {
        boss->autorelease();
        return boss;
    }
    CC_SAFE_DELETE(boss);
    return nullptr;
}

bool Boss::initBoss() {
    // Sử dụng sprite frame của Gumba
    if (!Sprite::initWithSpriteFrameName("castle-gumba-1"))
        return false;

    // Scale lên gấp đôi để làm boss
    this->setScale(2.5f);

    _state = BossState::Idle;
    _currentAction = nullptr;
    
    // Khởi tạo thông số di chuyển
    _moveSpeed = 30.0f; // Boss di chuyển chậm hơn Gumba một chút để có vẻ nặng nề hơn
    _velocity = Vec2(-_moveSpeed, 0.0f);
    _spawnX = 0;
    _leftLimit = 0;
    _rightLimit = 0;
    _movingLeft = true;

    playIdleAnimation();
    scheduleUpdate();

    return true;
}

void Boss::setState(BossState newState) {
    if (_state == newState)
        return;
    _state = newState;

    if (_currentAction) {
        stopAction(_currentAction);
        CC_SAFE_RELEASE_NULL(_currentAction);
    }

    switch (_state) {
    case BossState::Idle:
        playIdleAnimation();
        break;
    case BossState::Dead:
        playDeadAnimation();
        break;
    }
}

void Boss::playIdleAnimation() {
    // Boss Gumba sẽ có animation giống Gumba thường nhưng to hơn
    Vector<SpriteFrame*> frames;
    for (int i = 1; i <= 2; i++) {
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(
            StringUtils::format("castle-gumba-%d", i));
        if (frame)
            frames.pushBack(frame);
    }

    if (frames.size() > 0) {
        auto animation = Animation::createWithSpriteFrames(frames, 0.3f);
        auto animate = Animate::create(animation);
        _currentAction = RepeatForever::create(animate);
        _currentAction->retain();
        runAction(_currentAction);
    }
}

void Boss::playDeadAnimation() {
    // Stop animation hiện tại
    if (_currentAction) {
        stopAction(_currentAction);
        CC_SAFE_RELEASE_NULL(_currentAction);
    }
    
    // Set sprite frame chết của Gumba
    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("castle-gumba-3");
    if (frame)
        setSpriteFrame(frame);

    // Animation chết
    runAction(Sequence::create(
        ScaleTo::create(0.15f, 2.5f, 0.8f), // Bẹp xuống nhưng vẫn to
        FadeOut::create(0.25f),
        RemoveSelf::create(),
        nullptr
    ));
}

void Boss::setVelocity(const Vec2& v) {
    _velocity = v;
}

Vec2 Boss::getVelocity() const {
    return _velocity;
}

void Boss::setPatrolRange(float distance) {
    _spawnX = getPositionX();
    _leftLimit = _spawnX - distance;
    _rightLimit = _spawnX + distance;
}

void Boss::update(float dt) {
    // Boss đứng yên nếu đã chết
    if (_state == BossState::Dead)
        return;

    // Di chuyển trái phải như Gumba
    Vec2 pos = getPosition();

    if (_movingLeft) {
        pos.x -= _moveSpeed * dt;
        if (pos.x <= _leftLimit) {
            _movingLeft = false;
            setFlippedX(true); // Quay mặt sang phải
        }
    }
    else {
        pos.x += _moveSpeed * dt;
        if (pos.x >= _rightLimit) {
            _movingLeft = true;
            setFlippedX(false); // Quay mặt sang trái
        }
    }

    setPosition(pos);
}

