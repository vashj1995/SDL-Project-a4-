#pragma once
#ifndef __BULLET__
#define __BULLET__

#include "DisplayObject.h"
#include "Util.h"

class FeatherBullet : public DisplayObject
{
public:
	FeatherBullet(float x, float y, float r);
	~FeatherBullet();
	void draw() override;
	void update() override;
	void clean() override;
	void Move();
	glm::vec2 getOrientation()const;
	void setOrientation(glm::vec2 Orientation);
	void setRotation(float Angle);
	float getRotation()const;
	float getAccelerationRate()const;
	void setAccelerationRate(float rate);
private:
	glm::vec2 m_Orientation;
	float m_RotationAngle;
	float m_MaxSpeed;
	float m_accelerationRate;

};
#endif // !__BULLET__