#include "FeatherBullet.h"
#include "TextureManager.h"
#include "Game.h"
FeatherBullet::FeatherBullet(float x, float y, float r) : m_Orientation(glm::vec2(0.0f, -1.0f)), m_RotationAngle(r), m_accelerationRate(0.0f), m_MaxSpeed(2.0f)
{
	//TextureManager::Instance()->load("png", "Spear");
	//auto size = TextureManager::Instance()->getTextureSize("Spear");

	//setWidth(size.x);
	//setHeight(size.y);
	getTransform()->position = glm::vec2(x, y);
	getRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	getRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	m_MaxSpeed = 1.0f;
	getRigidBody()->isColliding = false;
	setType(BULLET);
}

FeatherBullet::~FeatherBullet()
= default;

void FeatherBullet::draw()
{
	//TextureManager::Instance()->draw("Spear", getTransform()->position.x, getTransform()->position.y, m_RotationAngle, 255, false);
}

void FeatherBullet::update()
{
	Move();
}

void FeatherBullet::clean()
{
}

void FeatherBullet::Move()
{
	auto deltalTime = TheGame::Instance()->getDeltaTime();
	getRigidBody()->acceleration = getOrientation() * getAccelerationRate();
	// using the formula pf = pi + vi*t_0.5ai*t^2
	getRigidBody()->velocity += getOrientation() * (deltalTime)+0.5f * getRigidBody()->acceleration * (deltalTime);
	getRigidBody()->velocity = Util::clamp(getRigidBody()->velocity, m_MaxSpeed);
	getTransform()->position += getRigidBody()->velocity;
}


glm::vec2 FeatherBullet::getOrientation() const
{
	return m_Orientation;
}

void FeatherBullet::setOrientation(glm::vec2 Orientation)
{
	m_Orientation = Orientation;
}

void FeatherBullet::setRotation(float Angle)
{
	m_RotationAngle = Angle;
	const auto angle_in_radians = (Angle)*Util::Deg2Rad;

	const auto x = cos(angle_in_radians);
	const auto y = sin(angle_in_radians);

	// convert the angle to a normalized vector
	setOrientation(glm::vec2(x, y));
}

float FeatherBullet::getRotation() const
{
	return m_RotationAngle;
}

float FeatherBullet::getAccelerationRate() const
{
	return m_accelerationRate;
}

void FeatherBullet::setAccelerationRate(float rate)
{
	m_accelerationRate = rate;
}