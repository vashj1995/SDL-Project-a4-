#pragma once
#ifndef __AGENT__
#define __AGENT__

#include <glm/vec4.hpp>
#include "DisplayObject.h"
#include "Health.h"
#include "EventManager.h"
#include "AgentType.h"

class Agent : public DisplayObject
{
public:
	Agent();
	~Agent();

	// Inherited via GameObject
	void draw() override = 0;
	void update() override = 0;
	void clean() override = 0;

	// getters
	glm::vec2 getTargetPosition() const;
	glm::vec2 getCurrentDirection() const;
	float getLOSDistance() const;
	bool hasLOS() const;
	float getCurrentHeading() const;
	glm::vec4 getLOSColour() const;

	float getDetectionDistance() const;
	bool hasDetection() const;
	glm::vec4 getDetectionColor() const;
	bool getDebugState() const;
	bool getMoving() const;
	bool getXMoving() const;
	bool getYMoving() const;
	AgentType getAgentType() const;
	int getHealth();
	bool getCloseCombat() const;
	bool getHealthState() const;
	bool getRangedAttackState() const;
	bool getInRange() const;
	bool getBehindCoverState() const;
	bool getHitState() const;

	// setters
	void setTargetPosition(glm::vec2 new_position);
	void setCurrentDirection(glm::vec2 new_direction);
	void setLOSDistance(float distance);
	void setHasLOS(bool state);
	void setCurrentHeading(float heading);
	void setLOSColour(glm::vec4 colour);

	void setDetectionDistance(float distance);
	void setHasDetection(bool state);
	void setDetectionColor(glm::vec4 colour);
	void setDebug(bool state);
	void setHealth(int health);
	void setMoving(bool state);
	void setXMoving(bool state);
	void setYMoving(bool state);
	void setHealthPostion(glm::vec2 postion);
	void drawHeath();

	void setAgentType(AgentType type);
	void setCloseCombat(bool state);
	void setHealthState(bool state);
	void setRangedAttackState(bool state);
	void setBehindCoverState(bool state);
	void setHitState(bool state);
	void setInrange(bool state);
	void SetHitTimer(int timer);
	int getHitTimer();

	//Firing Stuffs

	//Getters

	int getFireDistance();
	bool getisInFireDistance();
	glm::vec4 getFireDetectionColour();
	int getMinFireDistance();	
	glm::vec4 getMinFireDistancecolour();

	//Setters

	void setFireDistance(int distance);
	void setFireDetectionColour(glm::vec4 colour);
	void setIsInFireDetection(bool state);
	void setMinFireDistance(int distance);
	void setminFireDistanceColour(glm::vec4 colour);

private:
	void m_changeDirection();
	float m_currentHeading;
	glm::vec2 m_currentDirection;
	glm::vec2 m_targetPosition;

	// LOS
	float m_LOSDistance;
	bool m_hasLOS;
	glm::vec4 m_LOSColour;

	//pointers
	Health* m_agentHealth;
	AgentType m_Type;

	//Health, Combat stuff. 
	bool m_CloseCombatRange;
	bool m_HealthState;
	bool m_RangedAttack;
	bool m_inRangeDistance; 
	bool m_isBehindCover;
	bool m_isRecentlyHit;
	int m_RecentlyHitTimer;



	//Detection Radius
	float m_DetectionDistance;
	bool m_hasDetection;
	glm::vec4 m_DetectionColour;

	//Firing
	int m_fireDistance;
	bool m_isInFireDistance;
	glm::vec4 m_FireDistanceColour;
	int m_MinFireDistance;
	glm::vec4 m_minFireDistanceColour;

	//Debug
	bool m_DebugEnabled;
	bool m_isMoving;
	bool m_isXMoving;
	bool m_isYMoving;
};



#endif /* defined ( __AGENT__) */