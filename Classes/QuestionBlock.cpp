#include "QuestionBlock.h"
#include "SimpleAudioEngine.h"   
using namespace CocosDenshion;

QuestionBlock* QuestionBlock::createWithItemType(const std::string& itemType) {
    auto block = new QuestionBlock();
    if (block && block->initWithItemType(itemType)) {
        block->autorelease();
        return block;
    }
    CC_SAFE_DELETE(block);
    return nullptr;
}

bool QuestionBlock::initWithItemType(const std::string& itemType) {
    if (!Sprite::initWithSpriteFrameName("bonus-block-blink-1"))
        return false;

    _itemType = itemType;
    _state = BlockState::Idle;
    _currentAction = nullptr;

    playIdleAnimation();

    return true;
}

void QuestionBlock::playIdleAnimation() {
    if (_currentAction) {
        stopAction(_currentAction);
        CC_SAFE_RELEASE_NULL(_currentAction);
    }

    Vector<SpriteFrame*> frames;
    for (int i = 1; i <= 3; ++i) {
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(
            StringUtils::format("bonus-block-blink-%d", i));
        if (frame)
            frames.pushBack(frame);
    }

    auto animation = Animation::createWithSpriteFrames(frames, 0.2f);
    auto animate = Animate::create(animation);
    _currentAction = RepeatForever::create(animate);
    _currentAction->retain();
    runAction(_currentAction);
}

void QuestionBlock::playHitAnimation() {
    if (_currentAction) {
        stopAction(_currentAction);
        CC_SAFE_RELEASE_NULL(_currentAction);
    }

    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("bonus-block-empty");
    if (frame)
        setSpriteFrame(frame);

    //Block animation
    auto moveUp = MoveBy::create(0.08f, Vec2(0, 6));
    auto moveDown = MoveBy::create(0.08f, Vec2(0, -6));

    //Spawn item
    auto spawnItem = CallFunc::create([this]() {
        Vec2 spawnPos = this->getPosition() + Vec2(0, 16);
        auto parent = this->getParent();
        if (!parent) return;

        // Coin animation
        if (_itemType == "coin")
        {
            Vector<SpriteFrame*> frames;
            for (int i = 1; i <= 4; ++i) {
                auto f = SpriteFrameCache::getInstance()->getSpriteFrameByName(
                    StringUtils::format("coin-%d", i));
                if (f) frames.pushBack(f);
            }

            auto coin = Sprite::createWithSpriteFrame(frames.front());
            coin->setPosition(spawnPos);
            parent->addChild(coin, 20);

            auto anim = Animate::create(Animation::createWithSpriteFrames(frames, 0.1f));
            auto move = MoveBy::create(0.6f, Vec2(0, 40));
            auto fade = FadeOut::create(0.6f);

            coin->runAction(Sequence::create(
                Spawn::create(anim, move, nullptr),
                fade,
                RemoveSelf::create(),
                nullptr
            ));

            SimpleAudioEngine::getInstance()->playEffect("music/coin.mp3");
        }

        // Flower animation
        else if (_itemType == "flower")
        {
            Vector<SpriteFrame*> frames;
            for (int i = 1; i <= 4; ++i) {
                auto f = SpriteFrameCache::getInstance()->getSpriteFrameByName(
                    StringUtils::format("flower-%d", i));
                if (f) frames.pushBack(f);
            }

            auto flower = Sprite::createWithSpriteFrame(frames.front());
            flower->setPosition(this->getPosition());
            parent->addChild(flower, 0);

            auto anim = RepeatForever::create(Animate::create(Animation::createWithSpriteFrames(frames, 0.15f)));
            flower->runAction(anim);

            auto moveUp = MoveBy::create(0.6f, Vec2(0, 8));
            flower->runAction(moveUp);
            flower->setName("flower");

            SimpleAudioEngine::getInstance()->playEffect("music/powerup-appears.mp3");
        }

        // Star animation
        else if (_itemType == "star")
        {
            Vector<SpriteFrame*> frames;
            for (int i = 1; i <= 4; ++i) {
                auto f = SpriteFrameCache::getInstance()->getSpriteFrameByName(
                    StringUtils::format("star-%d", i));
                if (f) frames.pushBack(f);
            }

            auto star = Sprite::createWithSpriteFrame(frames.front());
            star->setPosition(this->getPosition());
            parent->addChild(star, 0);
            star->setName("star");

            auto anim = RepeatForever::create(Animate::create(Animation::createWithSpriteFrames(frames, 0.15f)));
            star->runAction(anim);

            auto moveUp = MoveBy::create(0.6f, Vec2(0, 24));
            star->runAction(moveUp);

            SimpleAudioEngine::getInstance()->playEffect("music/powerup-appears.mp3");
        }
        });
    runAction(Sequence::create(
        moveUp,
        spawnItem,
        moveDown,
        nullptr
    ));

    _state = BlockState::Empty;

}

void QuestionBlock::setState(BlockState newState) {
    if (_state == newState) return;
    _state = newState;

    switch (_state) {
    case BlockState::Idle:
        playIdleAnimation();
        break;
    case BlockState::Hit:
        playHitAnimation();
        break;
    case BlockState::Empty:
    {
        if (_currentAction) {
            stopAction(_currentAction);
            CC_SAFE_RELEASE_NULL(_currentAction);
        }

        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("bonus-block-empty");
        if (frame)
            setSpriteFrame(frame);

        _state = BlockState::Empty; 
        break;
    }
    }
}
