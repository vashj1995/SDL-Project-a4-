#include "Huntress.h"


#include "glm/gtx/string_cast.hpp"
#include "PlayScene.h"
#include "TextureManager.h"
#include "Util.h"

Huntress::Huntress() : m_maxSpeed(10.0f), m_currentAnimationState(PLAYER_IDLE)
{
	TextureManager::Instance()->loadSpriteSheet(
		"../Assets/sprites/atlas.txt",
		"../Assets/sprites/atlas.png", 
		"spriteSheet");

	setSpriteSheet(TextureManager::Instance()->getSpriteSheet("spriteSheet"));

	//Defining frame width and height
	setWidth(90);
	setHeight(70);

	getTransform()->position = glm::vec2(400.0f, 300.0f);
	getRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	getRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	getRigidBody()->isColliding = false;
	setType(PLAYER);

	setCurrentHeading(0.0f); // current facing angle
	setCurrentDirection(glm::vec2(1.0f, 0.0f)); // facing right
	m_turnRate = 5.0f; // 5 degrees per frame

	setMaxSpeed(5.0f);
	setLOSDistance(400.0f); // 5 ppf x 80 feet
	setLOSColour(glm::vec4(1, 0, 0, 1));
	setHasLOS(false);
	setDetectionDistance(60.0f);
	setDetectionColor(glm::vec4(0, 0, 1, 1));
	setHasDetection(false);
	setDebug(false);
	setHealth(3);
	setHealthPostion(getTransform()->position - glm::vec2(40.0f, 25.0f));
	setAnimationState(PLAYER_IDLE);

	m_buildAnimations();
}


Huntress::~Huntress()
= default;

void Huntress::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	switch (m_currentAnimationState)
	{
	case PLAYER_IDLE:
		TextureManager::Instance()->playAnimation("spriteSheet", getAnimation("idle"), x, y, 0.10f, getCurrentHeading(), 255, false);
		break;									  
	case PLAYER_RUN:							  
		TextureManager::Instance()->playAnimation("spriteSheet", getAnimation("run"), x, y, 0.10f, getCurrentHeading(), 255, false);
		break;									   
	case PLAYER_SHOOT:							   
		TextureManager::Instance()->playAnimation("spriteSheet", getAnimation("shoot"), x, y, 0.10f, getCurrentHeading(), 255, false);
		break;									   
	case PLAYER_MELEE:							   
		TextureManager::Instance()->playAnimation("spriteSheet", getAnimation("melee"), x, y, 0.10f, getCurrentHeading(), 255, false);
		break;
	case PLAYER_DEATH:
		TextureManager::Instance()->playAnimation("spriteSheet", getAnimation("death"), x, y, 0.10f, getCurrentHeading(), 255, false);
		break;
	}


	if (getDebugState())
	{
		//Draw LOS
		Util::DrawLine(glm::vec2(getTransform()->position.x + getWidth() / 2, getTransform()->position.y + getHeight() / 2),
			(glm::vec2(getTransform()->position.x + getWidth() / 2, getTransform()->position.y + getHeight() / 2)) + getCurrentDirection() * getLOSDistance(), getLOSColour());

		// draw detection radius
		Util::DrawCircle(glm::vec2(getTransform()->position.x + getWidth() / 2, getTransform()->position.y + getHeight() / 2), getDetectionDistance(), getDetectionColor());
	}
	drawHeath();
}


void Huntress::update()
{
	if (getMoving())
		move();
	m_checkBounds();
	setHealthPostion(getTransform()->position - glm::vec2(40.0f, 25.0f));
	auto angle = (atan2(EventManager::Instance().getMousePosition().y - getTransform()->position.y, EventManager::Instance().getMousePosition().x - getTransform()->position.x)
		* 180.00 / 3.1415926);
	setCurrentHeading(angle);

		// State machine can go here. With parsing events.
		switch (m_currentAnimationState)
		{
		case PLAYER_IDLE_RIGHT:
			// Any actions here.
			// Parse transitions. Idle->Runs first.
			if (EventManager::Instance().isKeyDown(SDL_SCANCODE_A))
			{
				setAnimationState(PLAYER_RUN_LEFT);
			}
			else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_D))
			{
				setAnimationState(PLAYER_RUN_RIGHT);
			}
			break;
		case PLAYER_IDLE_LEFT:
			// Any actions here.
			// Parse transitions. Idle->Runs first.
			if (EventManager::Instance().isKeyDown(SDL_SCANCODE_A))
			{
				setAnimationState(PLAYER_RUN_LEFT);
			}
			else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_D))
			{
				setAnimationState(PLAYER_RUN_RIGHT);
			}
			break;
		case PLAYER_RUN_RIGHT:
			// Any actions here.
			// Parse transitions. Start with transition back to idle.
			if (EventManager::Instance().isKeyUp(SDL_SCANCODE_D))
			{
				setAnimationState(PLAYER_IDLE_RIGHT);
			}
			break;
		case PLAYER_RUN_LEFT:
			// Any actions here.
			// Parse transitions. Start with transition back to idle.
			if (EventManager::Instance().isKeyUp(SDL_SCANCODE_A))
			{
				setAnimationState(PLAYER_IDLE_LEFT);
			}
			break;
		}
}

void Huntress::clean() {}

void Huntress::turnRight()
{
	setCurrentHeading(getCurrentHeading() + m_turnRate);
	if (getCurrentHeading() >= 360)
		setCurrentHeading(getCurrentHeading() - 360.0f);;
}

void Huntress::turnLeft()
{
	setCurrentHeading(getCurrentHeading() - m_turnRate);
	if (getCurrentHeading() < 0)
		setCurrentHeading(getCurrentHeading() + 360.0f);
}

void Huntress::moveForward()
{
	getRigidBody()->velocity = getCurrentDirection() * m_maxSpeed;
}

void Huntress::moveBack()
{
	getRigidBody()->velocity = getCurrentDirection() * -m_maxSpeed;
}

void Huntress::move()
{
	getRigidBody()->velocity = Util::clamp(getRigidBody()->velocity, getMaxSpeed());
	if (getXMoving())
	{
		getRigidBody()->velocity.x = getCurrentDirection().x * m_maxSpeed;
		getTransform()->position.x += getRigidBody()->velocity.x;

	}
	if (getYMoving())
	{

		getRigidBody()->velocity.y = getCurrentDirection().y * m_maxSpeed;
		getTransform()->position.y += getRigidBody()->velocity.y;
	}

}

float Huntress::getMaxSpeed() const
{
	return m_maxSpeed;
}

void Huntress::setMaxSpeed(const float newSpeed)
{
	m_maxSpeed = newSpeed;
}

void Huntress::setAnimationState(PlayerAnimationState new_state)
{
	m_currentAnimationState = new_state;
}

PlayerAnimationState Huntress::getAnimationState()
{
	return m_currentAnimationState;
}

void Huntress::setSpriteSheet(SpriteSheet * sprite_sheet)
{
	m_CharacterAnimation = sprite_sheet;
}

void Huntress::setAnimation(const Animation & animation)
{
	if (!m_animationsExists(animation.name))
		m_pAnimations[animation.name] = animation;
}

bool Huntress::m_animationsExists(const std::string & id)
{
	return m_pAnimations.find(id) != m_pAnimations.end();
}

Animation& Huntress::getAnimation(const std::string & name)
{
	return m_pAnimations[name];
}

void Huntress::m_buildAnimations()
{
	Animation idleAnimation = Animation();

	idleAnimation.name = "idle";
	idleAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressIdle1"));
	idleAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressIdle2"));
	idleAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressIdle3"));
	idleAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressIdle4"));
	idleAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressIdle5"));
	idleAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressIdle6"));
	idleAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressIdle7"));
	idleAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressIdle8"));

	setAnimation(idleAnimation);

	Animation runAnimation = Animation();

	runAnimation.name = "run";
	runAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressRun1"));
	runAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressRun2"));
	runAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressRun3"));
	runAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressRun4"));
	runAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressRun5"));
	runAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressRun6"));
	runAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressRun7"));
	runAnimation.frames.push_back(m_CharacterAnimation->getFrame("huntressRun8"));

	setAnimation(runAnimation);
}

void Huntress::m_checkBounds()
{

	if (getTransform()->position.x + getWidth() > Config::SCREEN_WIDTH)
		getTransform()->position = glm::vec2(800.0f - getWidth(), getTransform()->position.y);

	if (getTransform()->position.x < 0)
		getTransform()->position = glm::vec2(0.0f, getTransform()->position.y);

	if (getTransform()->position.y + getHeight() > Config::SCREEN_HEIGHT)
		getTransform()->position = glm::vec2(getTransform()->position.x, 600.0f - getHeight());

	if (getTransform()->position.y < 0)
		getTransform()->position = glm::vec2(getTransform()->position.x, 0.0f);
}

void Huntress::m_reset()
{
	getRigidBody()->isColliding = false;
	const int halfWidth = getWidth() * 0.5f;
	const auto xComponent = rand() % (640 - getWidth()) + halfWidth + 1;
	const auto yComponent = -getHeight();
	getTransform()->position = glm::vec2(xComponent, yComponent);
}

