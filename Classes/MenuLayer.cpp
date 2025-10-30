#include "MenuLayer.h"
#include "GameLevelLayer.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* MenuLayer::createScene() {
    Scene* scene = Scene::create();
    Layer* layer = MenuLayer::create();
    scene->addChild(layer);
    return scene;
}

bool MenuLayer::init() {
    if (!Layer::init()) {
        return false;
    }

    // Load map as background
    TMXTiledMap* map = TMXTiledMap::create("map/level1.tmx");
    map->setPosition(Vec2(0, 0));
    this->addChild(map, 0);

    // Title
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    Label* titleLabel = Label::createWithSystemFont("SUPER MARIO", "Arial", 48);
    titleLabel->setTextColor(Color4B::YELLOW);
    titleLabel->enableOutline(Color4B::BLACK, 3);
    titleLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
                                origin.y + visibleSize.height * 0.8));
    this->addChild(titleLabel, 10);

    // Create menu
    createMenu();

    // Play background music
    SimpleAudioEngine::getInstance()->playBackgroundMusic("music/level1.mp3", true);

    return true;
}

void MenuLayer::createMenu() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Play button
    MenuItemFont* playItem = MenuItemFont::create("PLAY", 
        CC_CALLBACK_1(MenuLayer::playButtonCallback, this));
    playItem->setFontName("Arial");
    playItem->setFontSize(32);
    playItem->setColor(Color3B::GREEN);

    // Exit button
    MenuItemFont* exitItem = MenuItemFont::create("EXIT", 
        CC_CALLBACK_1(MenuLayer::exitButtonCallback, this));
    exitItem->setFontName("Arial");
    exitItem->setFontSize(32);
    exitItem->setColor(Color3B::RED);

    // Create menu
    Menu* menu = Menu::create(playItem, exitItem, nullptr);
    menu->alignItemsVerticallyWithPadding(30);
    menu->setPosition(Vec2(origin.x + visibleSize.width / 2,
                           origin.y + visibleSize.height / 2));
    this->addChild(menu, 10);
}

void MenuLayer::playButtonCallback(Ref* pSender) {
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameLevelLayer::createScene()));
}

void MenuLayer::exitButtonCallback(Ref* pSender) {
    Director::getInstance()->end();
}
