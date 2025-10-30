#pragma once
#include "cocos2d.h"

USING_NS_CC;

class PauseLayer : public Layer {
public:
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(PauseLayer);

    // Menu callbacks
    void resumeButtonCallback(Ref* pSender);
    void menuButtonCallback(Ref* pSender);

private:
    void createPauseMenu();
};
