#include "Player.h"
using namespace std;

USING_NS_CC;

static const string MARIO_PREFIX_NORMAL = "mario";
static const string MARIO_PREFIX_FIRE = "mario-fire";


bool Player::initWithSpriteFrameName(const std::string& frameName)
{
	if (!Sprite::initWithSpriteFrameName(frameName))
	{
		return false;
	}

	_isJumping = false;
	_isMoving = false;
	_isOnGround = false;
	_moveDirection = 0.0f;
	_canJump = true;
	_isInvincible = false;
	_desiredPosition = Vec2::ZERO;
	_velocity = Vec2::ZERO;
	


	_state = PlayerState::Idle;
	_currentAction = nullptr;

	return true;
}

static Animation* createAnimation(const std::string& prefix, int frameCount, float delay)
{
	Vector<SpriteFrame*> frames;
	char frameName[64];
	for (int i = 1; i <= frameCount; ++i)
	{
		sprintf(frameName, "%s%d", prefix.c_str(), i);
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
		if (frame)
			frames.pushBack(frame);
	}

	return Animation::createWithSpriteFrames(frames, delay);
}


// Accessors
void Player::setDesiredPosition(const Vec2& value)
{
	_desiredPosition = value;
}

Vec2 Player::getDesiredPosition()
{
	return _desiredPosition;
}

void Player::setVelocity(const Vec2& value)
{
	_velocity = value;
}

Vec2 Player::getVelocity()
{
	return _velocity;
}

void Player::setOnGroundFlag(const bool value)
{
	_isOnGround = value;
}

bool Player::getOnGroundFlag()
{
	return _isOnGround;
}

void Player::setIsMovingFlag(const bool value)
{
	_isMoving = value;
}

bool Player::getIsMovingFlag()
{
	return _isMoving;
}

void Player::setIsJumpingFlag(const bool value)
{
	_isJumping = value;
}

bool Player::getIsJumpingFlag()
{
	return _isJumping;
}

void Player::setMoveDirection(float value)
{
	_moveDirection = value;
}
float Player::getMoveDirection() const
{
	return _moveDirection;
}


Player* Player::createWithSpriteFrameName(const std::string& frameName)
{
	Player* sprite = new Player();
	if (sprite && sprite->initWithSpriteFrameName(frameName))
	{
		sprite->autorelease();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

void Player::playIdleAnimation()
{
	if (_currentAction)
	{
		stopAction(_currentAction);
		CC_SAFE_RELEASE_NULL(_currentAction);
	}
	string prefix = (_form == PlayerForm::Fire) ? MARIO_PREFIX_FIRE : MARIO_PREFIX_NORMAL;
	auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(prefix + "-stand-1");
	if (frame)
		setSpriteFrame(frame);

	_currentAction = nullptr;
}

void Player::playRunAnimation()
{
	if (_currentAction)
	{
		stopAction(_currentAction);
		CC_SAFE_RELEASE_NULL(_currentAction);
	}
	string prefix = (_form == PlayerForm::Fire) ? MARIO_PREFIX_FIRE : MARIO_PREFIX_NORMAL;
	auto anim = createAnimation(prefix + "-walk-", 3, 0.1f);
	auto animate = Animate::create(anim);
	_currentAction = RepeatForever::create(animate);
	_currentAction->retain();
	runAction(_currentAction);
}

void Player::playJumpAnimation()
{
	if (_currentAction)
	{
		stopAction(_currentAction);
		CC_SAFE_RELEASE_NULL(_currentAction);
	}
	string prefix = (_form == PlayerForm::Fire) ? MARIO_PREFIX_FIRE : MARIO_PREFIX_NORMAL;
	auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName((prefix + "-jump-1"));
	if (frame)
	{

		setSpriteFrame(frame);
	}


	_currentAction = nullptr;
}

void Player::setState(PlayerState newState)
{
	if (_state == newState) return;

	_state = newState;

	switch (_state)
	{
	case PlayerState::Idle:
		playIdleAnimation();
		break;
	case PlayerState::Run:
		playRunAnimation();
		break;
	case PlayerState::Jump:
		playJumpAnimation();
		break;
	}
}


void Player::update(float delta)
{
	// Declare forces
	// Gravity force
	Vec2 gravity = Vec2(0.0, -450.0);
	Vec2 gravityStep = delta * gravity;
	// Friction force ratio
	float frictionRatio = 0.90f;
	// Jump force
	Vec2 jumpForce = Vec2(0.0, 310.0);
	float jumpCutoff = 150.0;
	// Moving forward force
	Vec2 forwardMove = Vec2(800.0, 0.0);
	Vec2 forwardStep = forwardMove * delta;

	// Apply forces
	// Apply gravity force
	_velocity += gravityStep;
	// Apply friction force
	_velocity = Vec2(_velocity.x * frictionRatio, _velocity.y);
	// Apply jump force
	if (_isJumping && _isOnGround && _canJump)
	{
		_velocity += jumpForce;
		setState(PlayerState::Jump);
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/jump.wav");
		_canJump = false;
	}
	else if (!_isJumping && _isOnGround)
	{
		_canJump = true;
	}
	else if (!_isJumping && _velocity.y > jumpCutoff)
	{
		_velocity = Vec2(_velocity.x, jumpCutoff);
	}
	// Apply move forward force
	if (_isMoving) {
		if (_isOnGround && _state != PlayerState::Jump)
			setState(PlayerState::Run);
		_velocity += forwardStep * _moveDirection;
	}
	else
	{
		if (_isOnGround)
		{
			if (_state != PlayerState::Jump)
			{
				_velocity.x *= 0.85f;

				if (fabs(_velocity.x) < 5.0f)
				{
					_velocity.x = 0.0f;
					if (_state != PlayerState::Idle)
						setState(PlayerState::Idle);
				}
			}
		}
	}

	// Fasten the velocity in a range
	Vec2 minMovement = Vec2(-120.0, -450.0);
	Vec2 maxMovement = Vec2(120.0, 250.0);
	_velocity = _velocity.getClampPoint(minMovement, maxMovement);

	// Step for movement by velocity
	Vec2 stepVelocity = _velocity * delta;
	Vec2 movementVector = getPosition() + stepVelocity;

	// Update position
	this->setDesiredPosition(movementVector);
	this->setPosition(getDesiredPosition());

	if (_moveDirection != 0)
		setFlippedX(_moveDirection < 0);
}

void Player::setForm(PlayerForm form)
{
	_form = form;

	if (_form == PlayerForm::Fire)
	{
		_canShoot = true;

		this->setSpriteFrame("mario-fire-stand-1");

		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/powerup.mp3");
	}
	else
	{
		_canShoot = false;
		this->setSpriteFrame("mario-stand-1");
	}
}


Rect Player::getCollisionBoundingBox()
{
	// What are 3, 6?
	// CGRectInset shrinks a CGRect by the number of pixels specified
	// in the second and third arguments. So in this case, the width of
	// your collision bounding box will be six pixels smaller — three
	// on each side — than the bounding box based on the image file
	// you’re using.

	//Inset
	Rect collisionBox = Rect(getBoundingBox().origin.x + 3,
		getBoundingBox().origin.y,
		getBoundingBox().size.width - 6,
		getBoundingBox().size.height);

	Vec2 diff = _desiredPosition - getPosition();

	//Offset
	Rect returnBoundingBox = Rect(collisionBox.origin.x + diff.x,
		collisionBox.origin.y + diff.y,
		collisionBox.size.width,
		collisionBox.size.height);

	return returnBoundingBox;
}
