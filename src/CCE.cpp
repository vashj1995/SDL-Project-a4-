#include "CCE.h"

#include "Game.h"
#include "glm/gtx/string_cast.hpp"
#include "PlayScene.h"
#include "TextureManager.h"
#include "Util.h"

//CCE = Close Combat Enemy

CCE::CCE()
{
	m_maxSpeed = 10.0f;
	/*TextureManager::Instance()->loadSpriteSheet("txt", "png", "CCE");
	setSpriteSheet(TextureManager::Instance()->getSpriteSheet("CCE"));*/

	//Set to appropriate, obv
	setWidth(30);
	setHeight(50);

	getTransform()->position = glm::vec2(400.0f, 300.0f);
	getRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	getRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	getRigidBody()->isColliding = false;
	setType(SHIP);

	setCurrentHeading(0.0f); // current facing angle
	setCurrentDirection(glm::vec2(1.0f, 0.0f)); // facing right
	m_turnRate = 5.0f; // 5 degrees per frame
	m_accelerationRate = 1.0f;
	m_maxSpeed = 3.0f;

	setLOSDistance(400.0f); // 5 ppf x 80 feet
	setLOSColour(glm::vec4(1, 0, 0, 1));
	setHasLOS(false);
	setDetectionDistance(200.0f);
	setDetectionColor(glm::vec4(0, 0, 1, 1));
	setHasDetection(false);
	setHealth(3);
	setHealthPostion(getTransform()->position - glm::vec2(40.0f, 25.0f));
	setAnimationState(ENEMY_RUN);
	setAgentType(CLOSE_COMBAT_ENEMY);

	setFireDistance(50.0f); //Can fire in this range
	setIsInFireDetection(false);

	deathCooldown = 60;

	decisionTree = new DecisionTree();
	decisionTree->setAgent(this);
	Hitbox = new SDL_FRect({ getTransform()->position.x, getTransform()->position.y, 64.0f, 64.0f });
	//m_buildAnimations();
}


CCE::~CCE()
= default;

void CCE::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	//switch (m_currentAnimationState)
	//{
	//case ENEMY_IDLE:
	//	TextureManager::Instance()->playAnimation("CCE", getAnimation("idle"), x, y, 0.10f, getCurrentHeading() + 80.0f, 255, false, SDL_FLIP_VERTICAL);
	//	break;

	//case ENEMY_RUN:
	//	TextureManager::Instance()->playAnimation("CCE", getAnimation("run"), x, y, 0.10f, getCurrentHeading() + 80.0f, 255, false, SDL_FLIP_VERTICAL);
	//	break;

	//case ENEMY_MELEE:
	//	TextureManager::Instance()->playAnimation("CCE", getAnimation("melee"), x, y, 0.20f, getCurrentHeading() + 80.0f, 255, false, SDL_FLIP_VERTICAL);
	//	break;

	//case ENEMY_DAMAGE:
	//	TextureManager::Instance()->playAnimation("CCE", getAnimation("damage"), x, y, 0.10f, getCurrentHeading() + 80.0f, 255, false, SDL_FLIP_VERTICAL);
	//	break;

	//case ENEMY_DEATH:
	//	TextureManager::Instance()->playAnimation("CCE", getAnimation("death"), x, y, 0.10f, getCurrentHeading() + 80.0f, 255, false, SDL_FLIP_VERTICAL);
	//	break;
	//}

	// draw LOS
	if (getDebugState())
	{
		Util::DrawLine(glm::vec2(getTransform()->position.x + getWidth() / 2, getTransform()->position.y + getHeight() / 2),
			(glm::vec2(getTransform()->position.x + getWidth() / 2, getTransform()->position.y + getHeight() / 2)) + getCurrentDirection() * getLOSDistance(), getLOSColour());

		// draw detection radius
/*		Util::DrawCircle(glm::vec2(getTransform()->position.x + getWidth() / 2, getTransform()->position.y + getHeight() / 2), getDetectionDistance(), getDetectionColor());

		Util::DrawCircle(glm::vec2(getTransform()->position.x + getWidth() / 2, getTransform()->position.y + getHeight() / 2), 50, getDetectionColor())*/;
	}

	drawHeath();
}


void CCE::update()
{
	glm::vec2 m_targetDistance = glm::vec2(abs(getTransform()->position.x - getTargetPosition().x), abs(getTransform()->position.y - getTargetPosition().y));
	float magnitudeDistance = sqrt((m_targetDistance.x * m_targetDistance.x) + (m_targetDistance.y * m_targetDistance.y));
	setHealthPostion(getTransform()->position - glm::vec2(40.0f, 25.0f));
	Hitbox->x = getTransform()->position.x;
	Hitbox->y = getTransform()->position.y;
}

void CCE::clean()
{
}

void CCE::Attack()
{
	//setAnimationState(CCENEMY_ATTACK);	
}

void CCE::m_buildAnimations()
{
	
}