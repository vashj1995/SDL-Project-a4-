#pragma once
#ifndef __RADIUS_CONDITION__
#define __RADIUS_CONDITION__
#include "ConditionNode.h"

class RadiusCondition : public ConditionNode
{
public:
	RadiusCondition(bool within_radius = false);
	virtual ~RadiusCondition();

	void setIsWithinRadius(bool state);

	virtual bool Condition() override;
private:
	bool m_isWithinRadius;
};

#endif
