#include "RCE.h"

#include "Game.h"
#include "glm/gtx/string_cast.hpp"
#include "PlayScene.h"
#include "TextureManager.h"
#include "Util.h"

//RANGED COMBAT ONLY! RCE = Ranged Combat Enemy

RCE::RCE()
{
	m_maxSpeed = 10.0f;
	/*TextureManager::Instance()->loadSpriteSheet("txt", "png", "RCE");
	setSpriteSheet(TextureManager::Instance()->getSpriteSheet("RCE"));*/

	//Set to appropriate, obv
	setWidth(25);
	setHeight(40);

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
	deathCooldown = 60;

	setAgentType(RANGED_ENEMY);

	setLOSDistance(400.0f); // 5 ppf x 80 feet
	setLOSColour(glm::vec4(1, 0, 0, 1));
	setHasLOS(false);
	setDetectionDistance(getLOSDistance() - 50.0f); //Detection
	setDetectionColor(glm::vec4(0, 0, 1, 1));
	setHasDetection(false);
	setHealth(3);
	setHealthPostion(getTransform()->position - glm::vec2(40.0f, 25.0f));
	setAnimationState(ENEMY_IDLE);
	//m_buildAnimations();

	setFireDetectionColour(glm::vec4(1, 0, 0, 1));
	setFireDistance(getLOSDistance() - 100.0f); //Can fire in this range
	setIsInFireDetection(false);

	//Minimum firing distance
	setMinFireDistance(getLOSDistance() - 200.0f);
	setminFireDistanceColour(glm::vec4(0, 0, 1, 1));
	setInrange(false);

	decisionTree = new DecisionTree();
	decisionTree->setAgent(this);
}


RCE::~RCE()
= default;

void RCE::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the ship
	/*switch (m_currentAnimationState)
	{
	case ENEMY_IDLE:
		TextureManager::Instance()->playAnimation("RCE", getAnimation("idle"), x, y, 0.10f, getCurrentHeading() + 80.0f, 255, false);
		break;

	case ENEMY_RUN:
		TextureManager::Instance()->playAnimation("RCE", getAnimation("run"), x, y, 0.10f, getCurrentHeading() + 80.0f, 255, false);
		break;

	case ENEMY_DAMAGE:
		TextureManager::Instance()->playAnimation("RCE", getAnimation("damage"), x, y, 0.10f, getCurrentHeading() + 80.0f, 255, false);
		break;

	case ENEMY_DEATH:
		TextureManager::Instance()->playAnimation("RCE", getAnimation("death"), x, y, 0.10f, getCurrentHeading() + 80.0f, 255, false);
		break;
	}*/

	// draw LOS
	if (getDebugState())
	{
		Util::DrawLine(glm::vec2(getTransform()->position.x + getWidth() / 2, getTransform()->position.y + getHeight() / 2),
			(glm::vec2(getTransform()->position.x + getWidth() / 2, getTransform()->position.y + getHeight() / 2)) + getCurrentDirection() * getLOSDistance(), getLOSColour());

		// draw detection radius
		Util::DrawCircle(glm::vec2(getTransform()->position.x + getWidth() / 2, getTransform()->position.y + getHeight() / 2), getFireDistance(), getFireDetectionColour());
		Util::DrawCircle(glm::vec2(getTransform()->position.x + getWidth() / 2, getTransform()->position.y + getHeight() / 2), getDetectionDistance(), getDetectionColor());

		Util::DrawCircle(glm::vec2(getTransform()->position.x + getWidth() / 2, getTransform()->position.y + getHeight() / 2), getMinFireDistance(), getMinFireDistancecolour());
	}

	drawHeath();
}


void RCE::update()
{
	glm::vec2 m_targetDistance = glm::vec2(abs(getTransform()->position.x - getTargetPosition().x), abs(getTransform()->position.y - getTargetPosition().y));
	float magnitudeDistance = sqrt((m_targetDistance.x * m_targetDistance.x) + (m_targetDistance.y * m_targetDistance.y));
	setHealthPostion(getTransform()->position - glm::vec2(40.0f, 25.0f));
}

void RCE::clean()
{
}


void RCE::Attack()
{

}

void RCE::m_buildAnimations()
{
	
}
