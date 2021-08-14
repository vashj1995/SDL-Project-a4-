#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "Button.h"
#include "Label.h"
#include "Obstacle.h"
#include "ship.h"
//#include "StarShip.h"
#include "Target.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene();

	// Scene LifeCycle Functions
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;

	//void moveStarShip() const;
private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	glm::vec2 m_mousePosition;
	bool m_isGridEnabled;

	// Game Objects
	Target* m_pTarget;
	Ship* m_pShip;
	Obstacle* m_pObstacle1;
	Obstacle* m_pObstacle2;
	Obstacle* m_pObstacle3;
	void m_CheckShipLOS(DisplayObject* object);

	// UI Controls
	bool m_bToggleGrid;
};

#endif /* defined (__PLAY_SCENE__) */