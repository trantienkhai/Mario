#include "PauseLayer.h"
#include "MenuLayer.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* PauseLayer::createScene() {
    Scene* scene = Scene::create();
    Layer* layer = PauseLayer::create();
    scene->addChild(layer);
    return scene;
}

bool PauseLayer::init() {
    if (!Layer::init()) {
        return false;
    }

    // Blue sky background (same as game)
    LayerColor* blueSky = LayerColor::create(Color4B(100, 100, 250, 255));
    this->addChild(blueSky, 0);

    // Load map as background (same as game)
    TMXTiledMap* map = TMXTiledMap::create("map/level1.tmx");
    map->setPosition(Vec2(0, 0));
    map->setOpacity(100); // Mờ đi để tạo hiệu ứng pause
    this->addChild(map, 1);

    // Semi-transparent overlay for better text visibility
    LayerColor* overlay = LayerColor::create(Color4B(0, 0, 0, 100));
    this->addChild(overlay, 2);

    // Title
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    Label* titleLabel = Label::createWithSystemFont("PAUSED", "Arial", 36);
    titleLabel->setTextColor(Color4B::YELLOW);
    titleLabel->enableOutline(Color4B::BLACK, 3);
    titleLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
                                origin.y + visibleSize.height * 0.7));
    this->addChild(titleLabel, 10);

    // Create pause menu
    createPauseMenu();

    return true;
}

void PauseLayer::createPauseMenu() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Resume button
    MenuItemFont* resumeItem = MenuItemFont::create("RESUME", 
        CC_CALLBACK_1(PauseLayer::resumeButtonCallback, this));
    resumeItem->setFontName("Arial");
    resumeItem->setFontSize(28);
    resumeItem->setColor(Color3B::GREEN);

    // Menu button
    MenuItemFont* menuItem = MenuItemFont::create("MENU", 
        CC_CALLBACK_1(PauseLayer::menuButtonCallback, this));
    menuItem->setFontName("Arial");
    menuItem->setFontSize(28);
    menuItem->setColor(Color3B::BLUE);

    // Create menu
    Menu* menu = Menu::create(resumeItem, menuItem, nullptr);
    menu->alignItemsVerticallyWithPadding(30);
    menu->setPosition(Vec2(origin.x + visibleSize.width / 2,
                           origin.y + visibleSize.height / 2));
    this->addChild(menu, 10);
}

void PauseLayer::resumeButtonCallback(Ref* pSender) {
    Director::getInstance()->popScene();
}

void PauseLayer::menuButtonCallback(Ref* pSender) {
    Director::getInstance()->popToRootScene();
    Director::getInstance()->replaceScene(MenuLayer::createScene());
}
