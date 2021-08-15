#pragma once
#ifndef __MAP_NODE__
#define __MAP_NODE__
#include "Agent.h"
#include "DisplayObject.h"
#include "TextureManager.h"
#include "Label.h"

class Node : public Agent
{
public:
	Node(int x, int y);

	~Node();

	void draw() override;
	void update() override;
	void clean() override;

	void setHasLOS(bool state);
	bool getHasLOS();

	bool m_hasLOS;
	Label* m_label;
	SDL_Color colour = { 255, 0, 0, 255 };
};

#endif 
