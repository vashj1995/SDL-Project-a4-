#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "Button.h"
#include "Label.h"
#include "Obstacle.h"
#include "Huntress.h"
#include "Target.h"
#include "MapNode.h"
#include "Melee.h"
#include "FeatherBullet.h"
#include "RCE.h"
#include "CCE.h"
#include "DecisionTree.h"
#include <vector>



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
	void CollisionsUpdate();


	//void moveStarShip() const;
private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	glm::vec2 m_mousePosition;


	//Chicken Enemy
	std::vector<Enemy*> m_pEnemy;

	//Target
	Target* m_pTarget;

	//Huntress
	Huntress* m_pHunt;

	//Obstacles
	std::vector<Obstacle*> m_pObstacle;

	//labels 
	Label* m_pInstructions{};
	Label* m_UIScore;

	//Nodes
	std::vector<Node*> m_pNode;

	//Voids
	void m_CheckHuntLOS(DisplayObject* object);
	void m_CheckHuntDetection(DisplayObject* object);
	void m_CheckEnemyDetection(Enemy* enemy);
	void m_CheckEnemyLOS(Enemy* enemy);
	void m_setDebugMode(bool state);
	void m_setPatrolMode(bool state);
	void m_CheckEnemyHealth(Enemy* enemy);
	void m_CheckCloseCombatRange(Enemy* enemy);
	void m_CheckEnemyFireDetection(Enemy* enemy);
	void m_CheckNodeLOS(Node* node);
	void m_CheckBehindCover(Enemy* enemy);
	void m_CheckTooClose(Enemy* enemy);
	void m_RespawnEnemy(int numb);
	void m_setUIScore();
	void m_DecisionMaking(Enemy* m_agent);
	void m_CheckEnemyBehindCover(Enemy* enemy);



	//Bools
	bool m_isGridEnabled;
	bool m_isPatrolling;
	bool m_getDebugMode() const;
	bool m_getPatrolMode() const;
	bool m_CheckNodeEnemyLOS(Node* node, Agent* enemy);


	//DecisionTree* decisionTree;
	const float m_playerSpeed = 5.0f;

	//Melee attack
	Melee* m_meleeActtack;
	int meleeCoolDown = 30;

	//Bullets
	std::vector<FeatherBullet*> m_pPlayerBullets;
	std::vector<FeatherBullet*> m_pEnemyBullets;
	int currentMapNode;
	int m_enemysKilled = 0;

	//Cooldowns
	int cooldown = 20;
	int pfootstep = 0;
	int deathCooldown = 60;
	int damageCooldown = 60;
	int meleeDamageCooldown = 60;
	int footstepCooldown = 60;
	int EnemyFireCoolDown = 20;
	int CCEnemyAttackCoolDown = 5;

};

#endif /* defined (__PLAY_SCENE__) */