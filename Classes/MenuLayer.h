#pragma once
#include "cocos2d.h"

USING_NS_CC;

class MenuLayer : public Layer {
public:
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(MenuLayer);

    // Menu callbacks
    void playButtonCallback(Ref* pSender);
    void exitButtonCallback(Ref* pSender);

private:
    void createMenu();
};
