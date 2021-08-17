#pragma once
#ifndef __MELEE_ATTACK__
#define __MELEE_ATTACK__

#include "DisplayObject.h"

class Melee : public DisplayObject
{
public:
	Melee(float r);
	~Melee();
	void draw() override;
	void update() override;
	void clean() override;
	void setDirection(float direction);
private:
	float m_direction;
};
#endif // !__MELEE_ATTACK__

