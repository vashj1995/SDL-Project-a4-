#pragma once
#ifndef __CC_ENEMY__
#define __CC_ENEMY__

#include "Agent.h"
#include "TextureManager.h"
#include "PlayerAnimationState.h"
#include <glm/vec4.hpp>


#include "Enemy.h"
#include "Sprite.h"

class CCE final : public Enemy
{
public:
	CCE();
	~CCE();

	// Inherited via Agent
	void draw() override;
	void update() override;
	void clean() override;
	void Attack() override;

private:
	void m_buildAnimations() override;

};


#endif /* defined (__ENEMY__) */