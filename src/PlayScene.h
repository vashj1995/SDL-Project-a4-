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

	//Huntress
	//Huntress* m_pHunt;
	bool m_huntFacingRight;

	//Chicken
	//Enemy* m_pChicky;

	//Target
	Target* m_pTarget;

	//Obstacles
	Obstacle* m_pObstacle[5];

	//labels 
	Label* m_pInstructions{};
	Label* m_UIScore;
};

#endif /* defined (__PLAY_SCENE__) */