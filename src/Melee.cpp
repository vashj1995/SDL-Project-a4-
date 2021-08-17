#include "Melee.h"
#include "TextureManager.h"
#include "Util.h"

Melee::Melee(float r) :m_direction(r)
{
	//TextureManager::Instance()->load("png", "claw");
	//auto size = TextureManager::Instance()->getTextureSize("claw");
	//setWidth(size.x);
	//setHeight(size.y);
}

Melee::~Melee()
= default;

void Melee::draw()
{
	//TextureManager::Instance()->draw("claw", getTransform()->position.x, getTransform()->position.y, m_direction, 255, false);
}

void Melee::update()
{
}

void Melee::clean()
{
}

void Melee::setDirection(float direction)
{
	m_direction = direction;
}
