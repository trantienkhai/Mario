#include "GameLevelLayer.h"
#include "SimpleAudioEngine.h"   
#include "Fireball.h"
using namespace CocosDenshion;

USING_NS_CC;
Scene* GameLevelLayer::createScene()
{
	Scene* scene = Scene::create();
	Layer* layer = GameLevelLayer::create();
	scene->addChild(layer);
	return scene;
}



bool GameLevelLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	//Init plist
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Mario.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Gumba.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Block.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Coin.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Flower.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Star.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("FireBall.plist");

	// set background color
	LayerColor* blueSky = LayerColor::create(Color4B(100, 100, 250, 255));
	this->addChild(blueSky);

	// Create map
	_map = TMXTiledMap::create("map/level1.tmx");
	this->addChild(_map, 0);

	//Create QuestionBlock
	auto questionLayer = _map->getObjectGroup("questionBlocks");
	if (questionLayer)
	{
		auto& objects = questionLayer->getObjects();
		for (auto& obj : objects)
		{
			ValueMap props = obj.asValueMap();
			float x = props["x"].asFloat();
			float y = props["y"].asFloat();
			
			std::string itemType = "coin";
			if (props.find("Value") != props.end())
			{
				itemType = props["Value"].asString();
			}

			auto block = QuestionBlock::createWithItemType(itemType);
			block->setPosition(Vec2(x + 8, y + 8));
			_map->addChild(block, 1);
			_questionBlocks.push_back(block);
		}
	}

	// Init player
	_player = Player::createWithSpriteFrameName("mario-stand-1");
	_player->setPosition(Vec2(100, 50));
	_map->addChild(_player, 15);


	// Init data for layer
	_wallLayer = _map->getLayer("walls");
	_hazardLayer = _map->getLayer("hazards");

	// --- Spawn enemies from tile layer ---
	auto enemyLayer = _map->getLayer("enemies");
	if (enemyLayer)
	{
		enemyLayer->setVisible(false); // Hide enemies layer
		auto mapSize = _map->getMapSize();
		auto tileSize = _map->getTileSize();

		for (int x = 0; x < mapSize.width; ++x)
		{
			for (int y = 0; y < mapSize.height; ++y)
			{
				int gid = enemyLayer->getTileGIDAt(Vec2(x, y));
				if (gid != 0) // if exist
				{
					float posX = x * tileSize.width + tileSize.width / 2;
					float posY = (mapSize.height - y - 1) * tileSize.height + tileSize.height / 2;

					auto enemy = Enemy::createWithSpriteFrameName("castle-gumba-1");
					enemy->setPosition(Vec2(posX, posY));
					enemy->setPatrolRange(64.0f);
					_map->addChild(enemy, 10);
					_enemies.push_back(enemy);

				}
			}
		}
	}

	//Keyboard
	initKeyboardListener();

	// Autio
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("music/level1.mp3");

	// Run update
	this->scheduleUpdate();

	return true;
}

/**
 * Update handler function
 */
void GameLevelLayer::update(float delta) {
	if (_isGameOver)
	{
		return;
	}

	_player->update(delta);

	for (auto it = _enemies.begin(); it != _enemies.end(); )
	{
		Enemy* enemy = *it;
		if (!enemy)
		{
			it = _enemies.erase(it);
			continue;
		}
		if (enemy->getParent() == nullptr || enemy->getState() == EnemyState::Dead)
		{
			it = _enemies.erase(it);
			continue;
		}

		enemy->update(delta);
		++it;
	}

	this->handleHazardCollisions(_player);
	this->checkForWin();
	this->checkForAndResolveCollisions(_player);
	this->checkPlayerItemCollisions();
	this->checkPlayerEnemyCollisions();
	this->checkFireballEnemyCollisions();
	this->setViewpointCenter(_player->getPosition());	
}



void GameLevelLayer::initKeyboardListener()
{
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(GameLevelLayer::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(GameLevelLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void GameLevelLayer::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:
		_player->setIsMovingFlag(true);
		_player->setMoveDirection(1.0f);
		break;

	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:
		_player->setIsMovingFlag(true);
		_player->setMoveDirection(-1.0f);
		break;

	case EventKeyboard::KeyCode::KEY_SPACE:
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		_player->setIsJumpingFlag(true);
		break;

	case EventKeyboard::KeyCode::KEY_SHIFT:
		if (_player->canShoot())
		{
			bool facingRight = !_player->isFlippedX();
			auto fireball = Fireball::createFireball(facingRight, _map);
			Vec2 spawnPos = _player->getPosition() + Vec2(facingRight ? 16 : -16, 8);
			_map->addChild(fireball, 10);
			fireball->setPosition(spawnPos);
		}
		break;
	}
}

void GameLevelLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:
		_player->setIsMovingFlag(false);
		_player->setMoveDirection(0.0f);
		break;

	case EventKeyboard::KeyCode::KEY_SPACE:
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		_player->setIsJumpingFlag(false);
		break;
	}
}


Point GameLevelLayer::tileCoordForPosition(Point position)
{
	auto tileSize = _map->getTileSize();
	auto mapSize = _map->getMapSize();
	float x = floor(position.x / tileSize.width);
	float levelHeightInPixels = mapSize.height * tileSize.height;
	float y = floor((levelHeightInPixels - position.y) / tileSize.height);
	return Point(x, y);
}

/**
 * Get the Tile's Rect in Cocos from a tileCoordinates
 *   x: tileCoordinates.x * tileSize.width
 *   y: Map's height in pixel - (tileCoordinates.y + 1)*tileSize.height;
 * Note:
 *   tileCoordinate.y + 1: because tilemap is a base 0 indexing.
 */
Rect GameLevelLayer::tileRectFromTileCoords(Vec2 tileCoords)
{
	auto tileSize = _map->getTileSize();
	auto mapSize = _map->getMapSize();
	float levelHeightInPixels = mapSize.height * tileSize.height;
	Point origin = Point(tileCoords.x * tileSize.width, levelHeightInPixels - ((tileCoords.y + 1) * tileSize.height));
	return Rect(origin.x, origin.y, tileSize.width, tileSize.height);
}

/**
 * Get list of 8 surrounding tiles of player's position.
 */
std::vector<tileInfo> GameLevelLayer::getSurroundingTilesAtPosition(Vec2 position, TMXLayer* layer)
{
	// Player's position in tilemap
	Point playerPos = tileCoordForPosition(position);

	// List of player's surrounding tiles
	std::vector<tileInfo> gids;

	// Iterate over 9 tiles (8 surrounding tiles of player and player tile)
	for (int i = 0; i < 9; i++)
	{
		// Calculate the surrounding tile position in tilemap
		int col = i % 3;
		int row = (int)(i / 3);
		Vec2 surTilePos = Vec2(playerPos.x + (col - 1), playerPos.y + (row - 1));

		// Fall in a hole (over ground) => game over
		if (surTilePos.y > (_map->getMapSize().height - 1))
		{
			gameOver(false);
			return gids;
		}

		// Collided with layer's tile (eg: wall layer, hazard layer, ...)
		int tileGid = layer->getTileGIDAt(surTilePos);
		// Get world position of the surrounding tile
		Rect tileRect = tileRectFromTileCoords(surTilePos);

		// Fill all info to the surrounding tile
		tileInfo tileItem;
		tileItem.gid = tileGid;
		tileItem.x = tileRect.origin.x;
		tileItem.y = tileRect.origin.y;
		tileItem.tilePos = surTilePos;

		gids.push_back(tileItem);
	}

	// Priority of collided test: Bottom, Top, Left, Right
	gids.erase(gids.begin() + 4);
	gids.insert(gids.begin() + 6, gids[2]);
	gids.erase(gids.begin() + 2);

	tileInfo temp;
	temp = gids[6];
	gids[6] = gids[4];
	gids[4] = temp;

	temp = gids[0];
	gids[0] = gids[4];
	gids[4] = temp;

	return gids;
}


void GameLevelLayer::handleHazardCollisions(Player* player)
{
	std::vector<tileInfo> surTiles =
		getSurroundingTilesAtPosition(player->getPosition(), _hazardLayer);

	for (int i = 0; i < surTiles.size(); i++)
	{
		if (surTiles[i].gid)
		{
			Rect collTileRect = Rect(surTiles[i].x,
				surTiles[i].y,
				_map->getTileSize().width,
				_map->getTileSize().height);
			Rect playerRect = player->getCollisionBoundingBox();
			if (playerRect.intersectsRect(collTileRect))
			{
				gameOver(false);
			}
		}
	}
}



void GameLevelLayer::checkPlayerItemCollisions()
{
	Rect playerRect = _player->getCollisionBoundingBox();

	for (auto child : _map->getChildren())
	{
		std::string name = child->getName();
		if (name != "flower" && name != "star")
			continue;

		auto sprite = dynamic_cast<Sprite*>(child);
		if (!sprite) continue;

		Rect itemRect = sprite->getBoundingBox();
		if (!playerRect.intersectsRect(itemRect))
			continue;

		if (name == "flower")
		{
			_player->setForm(PlayerForm::Fire);
			SimpleAudioEngine::getInstance()->playEffect("music/powerup.mp3");
		}

		else if (name == "star")
		{
			if (!_player->isInvincible())
			{
				_player->setInvincible(true);
				SimpleAudioEngine::getInstance()->playEffect("music/powerup.mp3");

				auto blink = RepeatForever::create(Sequence::create(
					FadeOut::create(0.1f),
					FadeIn::create(0.1f),
					nullptr
				));
				_player->runAction(blink);

				_player->runAction(Sequence::create(
					DelayTime::create(8.0f),
					CallFunc::create([this]() {
						_player->setInvincible(false);
						_player->stopAllActions();
						_player->setOpacity(255);
						}),
					nullptr
				));
			}
		}

		sprite->removeFromParent();
		break;
	}
}

void GameLevelLayer::checkFireballEnemyCollisions()
{
	std::vector<Node*> fireballsToRemove;
	std::vector<Enemy*> enemiesToRemove;

	// Duyệt tất cả fireball đang tồn tại
	for (auto child : _map->getChildren())
	{
		if (child->getName() != "fireball")
			continue;

		auto fireball = dynamic_cast<Sprite*>(child);
		if (!fireball)
			continue;

		Rect fbRect = fireball->getBoundingBox();

		for (auto enemy : _enemies)
		{
			if (!enemy || enemy->getState() == EnemyState::Dead)
				continue;

			if (fbRect.intersectsRect(enemy->getBoundingBox()))
			{
				// Enemy bị tiêu diệt
				enemy->setState(EnemyState::Dead);
				SimpleAudioEngine::getInstance()->playEffect("music/kick.mp3");

				// Đánh dấu fireball để xóa sau
				fireballsToRemove.push_back(fireball);
				break;
			}
		}
	}

	// Xóa fireball sau khi duyệt xong (tránh invalid iterator)
	for (auto fb : fireballsToRemove)
	{
		fb->removeFromParent();
	}
}


/**
 * Check & Resolve collisions with the surrouding tiles of player
 *
 */
void GameLevelLayer::checkForAndResolveCollisions(Player* player)
{
	std::vector<tileInfo> surTiles = this->getSurroundingTilesAtPosition(player->getPosition(), _wallLayer);
	// Fall in a hole
	if (_isGameOver)
	{
		return;
	}

	player->setOnGroundFlag(false);

	for (int i = 0; i < surTiles.size(); ++i)
	{
		Rect playerRect = player->getCollisionBoundingBox();

		int gid = surTiles[i].gid;

		// Collided
		if (gid)
		{
			Rect collTileRect = Rect(surTiles[i].x,
				surTiles[i].y,
				_map->getTileSize().width,
				_map->getTileSize().height);
			if (playerRect.intersectsRect(collTileRect))
			{
				// This is replicating CGRectIntersection
				Rect intersection = Rect(std::max(playerRect.getMinX(), collTileRect.getMinX()),
					std::max(playerRect.getMinY(), collTileRect.getMinY()),
					0,
					0);
				intersection.size.width = std::min(playerRect.getMaxX(), collTileRect.getMaxX())
					- intersection.getMinX();
				intersection.size.height = std::min(playerRect.getMaxY(), collTileRect.getMaxY())
					- intersection.getMinY();

				switch (i)
				{
				case 0: // Bottom: directly below Koala
					player->setDesiredPosition(Vec2(player->getDesiredPosition().x,
						player->getDesiredPosition().y + intersection.size.height));
					player->setVelocity(Vec2(player->getVelocity().x, 0.0));
					player->setOnGroundFlag(true);
					if (player->getState() == PlayerState::Jump)
					{
						if (player->getIsMovingFlag())
							player->setState(PlayerState::Run);
						else
							player->setState(PlayerState::Idle);
					}
					break;
				case 1: // Top: directly above Koala
					player->setDesiredPosition(Vec2(player->getDesiredPosition().x,
						player->getDesiredPosition().y - intersection.size.height));
					player->setVelocity(Vec2(player->getVelocity().x, 0.0));
					break;
				case 2: // Left
					player->setDesiredPosition(Vec2(player->getDesiredPosition().x + intersection.size.width,
						player->getDesiredPosition().y));
					break;
				case 3: // Right
					player->setDesiredPosition(Vec2(player->getDesiredPosition().x - intersection.size.width,
						player->getDesiredPosition().y));
					break;
				default: // Other: 4 (Top Left), 5 (Top Right), 6 (Bottom Left), 7 (Bottom Right)
					// Vertical Collision
					if (intersection.size.width > intersection.size.height)
					{
						// tile is diagonal, but resolving collision vertically
						player->setVelocity(Vec2(player->getVelocity().x, 0.0));

						float resolutionHeight;
						if (i == 6 || i == 7)  // 6, 7: Bottom Left, Bottom Right tiles
						{
							resolutionHeight = intersection.size.height;
							player->setOnGroundFlag(true);
							if (player->getState() == PlayerState::Jump)
							{
								if (player->getIsMovingFlag())
									player->setState(PlayerState::Run);
								else
									player->setState(PlayerState::Idle);
							}
						}
						else // 4, 5: Top Left, Top Right tiles
						{
							resolutionHeight = -intersection.size.height;
						}
						player->setDesiredPosition(Vec2(player->getDesiredPosition().x,
							player->getDesiredPosition().y + resolutionHeight));
					}
					else // Horizontal Collision
					{
						//tile is diagonal, but resolving horizontally
						float resolutionWidth;

						if (i == 4 || i == 6) // 4: Top Left tile, 6: Top Right tile
						{
							resolutionWidth = intersection.size.width;
						}
						else // 5: Top Right, 7: Bottom Right
						{
							resolutionWidth = -intersection.size.width;
						}
						player->setDesiredPosition(Vec2(player->getDesiredPosition().x,
							player->getDesiredPosition().y + resolutionWidth));
					}
					break;
				}
			}
		}
	}

	// --- Handle collisions with QuestionBlocks ---
	for (auto block : _questionBlocks)
	{
		if (!block) continue;

		Rect playerRect = player->getCollisionBoundingBox();
		Rect blockRect = block->getBoundingBox();

		if (!playerRect.intersectsRect(blockRect))
			continue;

		Rect intersection = Rect(
			std::max(playerRect.getMinX(), blockRect.getMinX()),
			std::max(playerRect.getMinY(), blockRect.getMinY()),
			0, 0
		);
		intersection.size.width = std::min(playerRect.getMaxX(), blockRect.getMaxX()) - intersection.getMinX();
		intersection.size.height = std::min(playerRect.getMaxY(), blockRect.getMaxY()) - intersection.getMinY();

		float playerBottom = playerRect.getMinY();
		float playerTop = playerRect.getMaxY();
		float blockBottom = blockRect.getMinY();
		float blockTop = blockRect.getMaxY();

		if (player->getVelocity().y > 0 && playerTop <= blockBottom + 4)
		{
			player->setVelocity(Vec2(player->getVelocity().x, -80));
			player->setDesiredPosition(Vec2(
				player->getDesiredPosition().x,
				player->getDesiredPosition().y - intersection.size.height
			));

			if (block->getState() == BlockState::Idle)
				block->setState(BlockState::Hit);
		}
		else if (player->getVelocity().y <= 0 && playerBottom >= blockTop - 4)
		{
			player->setVelocity(Vec2(player->getVelocity().x, 0));
			player->setOnGroundFlag(true);
			player->setDesiredPosition(Vec2(
				player->getDesiredPosition().x,
				player->getDesiredPosition().y + intersection.size.height
			));

			if (player->getState() == PlayerState::Jump)
			{
				if (player->getIsMovingFlag())
					player->setState(PlayerState::Run);
				else
					player->setState(PlayerState::Idle);
			}
		}
		else
		{
			if (intersection.size.width < intersection.size.height)
			{
				if (playerRect.getMidX() < blockRect.getMidX())
					player->setDesiredPosition(Vec2(player->getDesiredPosition().x - intersection.size.width, player->getDesiredPosition().y));
				else
					player->setDesiredPosition(Vec2(player->getDesiredPosition().x + intersection.size.width, player->getDesiredPosition().y));
			}
		}
	}
	player->setPosition(player->getDesiredPosition());
}


void GameLevelLayer::checkPlayerEnemyCollisions()
{
	auto playerRect = _player->getCollisionBoundingBox();

	for (auto enemy : _enemies)
	{
		if (!enemy || enemy->getState() == EnemyState::Dead)
			continue;

		auto enemyRect = enemy->getBoundingBox();

		if (playerRect.intersectsRect(enemyRect))
		{

			float playerBottom = playerRect.getMinY();
			float enemyTop = enemyRect.getMaxY();

			if (_player->getVelocity().y < 0 && playerBottom > enemyTop - 8)
			{
				enemy->setState(EnemyState::Dead);

				_player->setVelocity(Vec2(_player->getVelocity().x, 180));

				SimpleAudioEngine::getInstance()->playEffect("music/kick.mp3");

				continue; 
			}
			else
			{
				if(_player->isInvincible())
				{
					enemy->setState(EnemyState::Dead);
					SimpleAudioEngine::getInstance()->playEffect("music/kick.mp3");
					_player->setInvincible(false);
					_player->stopAllActions();
					_player->setOpacity(255);
					continue;
				}
				gameOver(false);
				return;
			}
		}
	}
}

/**
 *
 */
void GameLevelLayer::setViewpointCenter(Vec2 position)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	int x = MAX(position.x, visibleSize.width / 2);
	int y = MAX(position.y, visibleSize.height / 2);
	x = MIN(x, (_map->getMapSize().width * _map->getTileSize().width)
		- visibleSize.width / 2);
	y = MIN(y, (_map->getMapSize().height * _map->getTileSize().height)
		- visibleSize.height / 2);
	Vec2 actualPosition = Vec2(x, y);

	Vec2 centerOfView = Vec2(visibleSize.width / 2, visibleSize.height / 2);
	Vec2 viewPoint = centerOfView - actualPosition;
	_map->setPosition(viewPoint);
}


/**
 *
 */
void GameLevelLayer::gameOver(bool playerDidWin)
{
	_isGameOver = true;
	std::string gameText;

	if (playerDidWin)
	{
		gameText = "You Won!";
	}
	else
	{
		gameText = "You have died!";
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/hurt.wav");
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Menu* pMenu = Menu::create();
	pMenu->setPosition(Vec2(visibleSize.width / 2, -50));

	MenuItemFont* diedLabel = MenuItemFont::create(gameText);
	diedLabel->setFontName("Marker Felt");
	diedLabel->setFontSize(24);
	diedLabel->setPosition(Vec2(0, 50));
	pMenu->addChild(diedLabel);

	MoveBy* slideIn = MoveBy::create(1.0, Vec2(0, 200));

	MenuItemImage* replay = MenuItemImage::create("replay.png", "replay.png", "replay.png");
	replay->setPosition(Point::ZERO);
	replay->setCallback(CC_CALLBACK_1(GameLevelLayer::replayButtonCallback, this));
	pMenu->addChild(replay);

	this->addChild(pMenu, 1);

	pMenu->runAction(slideIn);
}

void GameLevelLayer::replayButtonCallback(Ref* pSender)
{
	Director::getInstance()->replaceScene(GameLevelLayer::createScene());
}

void GameLevelLayer::checkForWin()
{
	if (_player->getPosition().x > 3130.0)
	{
		gameOver(true);
	}
}
