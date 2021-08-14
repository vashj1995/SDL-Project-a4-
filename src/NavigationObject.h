#pragma once
#ifndef __NAVIGATION_OBJECT__
#define __NAVIGATION_OBJECT__
#include "DisplayObject.h"

class NavigationObject : public DisplayObject
{
public:
	// Constructor
	NavigationObject();
	
	// Destructor
	~NavigationObject();

	// Inherited from DisplayObject
	void draw() override = 0;
	void update() override = 0;
	void clean() override = 0;

	// getters and setters
	glm::vec2 getGridPosition() const;
	void setGridPosition(float col, float row);

private:
	glm::vec2 m_gridPosition;
};

#endif /* defined (__NAVIGATION_OBJECT__) */