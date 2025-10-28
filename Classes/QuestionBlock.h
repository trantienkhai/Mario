#pragma once
#include "cocos2d.h"

USING_NS_CC;

enum class BlockState {
    Idle,
    Hit,
    Empty
};

class QuestionBlock : public Sprite {
public:
    static QuestionBlock* createWithItemType(const std::string& itemType);

    bool initWithItemType(const std::string& itemType);
    void playIdleAnimation();
    void playHitAnimation();

    void setState(BlockState newState);
    BlockState getState() const { return _state; }

    std::string getItemType() const { return _itemType; }

private:
    BlockState _state;
    std::string _itemType;
    Action* _currentAction;
};
