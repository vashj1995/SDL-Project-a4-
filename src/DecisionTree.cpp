#include "DecisionTree.h"
#include <iostream>


DecisionTree::DecisionTree() {}

DecisionTree::~DecisionTree() = default;

Agent* DecisionTree::getAgent() const
{
	return m_agent;
}

void DecisionTree::setAgent(Agent * agent)
{
	m_agent = agent;
	m_buildTree();
}

TreeNode* DecisionTree::getCurrentNode() const
{
	return m_currentNode;
}

void DecisionTree::setCurrentNode(TreeNode * node)
{
	m_currentNode = node;
}

ActionNode* DecisionTree::getCurrentAction() const
{
	return m_CurrentAction;
}

void DecisionTree::setCurrentAction(ActionNode * node)
{
	m_CurrentAction = node;
}

TreeNode* DecisionTree::AddNode(TreeNode * parent, TreeNode * child_node, const TreeNodeType type)
{
	switch (type)
	{
	case LEFT_TREE_NODE:
		parent->Left = child_node;
		break;
	case RIGHT_TREE_NODE:
		parent->Right = child_node;
		break;
	}

	child_node->Parent = parent; // Backwards Traversal

	return child_node;
}

void DecisionTree::Display()
{
	for (auto* node : m_treeNodeList)
	{
		std::cout << node->name << std::endl;
	}
}

void DecisionTree::Update()
{
	if (m_agent != nullptr)
	{
		m_LOSNode->setLOS(m_agent->hasLOS());

		m_RadiusNode->setIsWithinRadius(m_agent->hasDetection());

		m_HealthNode->setHealthCondition(m_agent->getHealthState());

		if (m_agent->getAgentType() == RANGED_ENEMY)
		{
			m_BehindCoverNode->setBehindCover(m_agent->getBehindCoverState());

			m_RecentlyHitNode->setRecentlyHit(m_agent->getHitState());

			m_AttackRangeNode->setAttackRange(m_agent->getRangedAttackState());

			m_RangeDistanceNode->setRangeDistance(m_agent->getInRange());
		}
		else if (m_agent->getAgentType() == CLOSE_COMBAT_ENEMY)
			m_CloseCombatNode->setIsWithinCombatRange(m_agent->getCloseCombat());

	}
}

std::string DecisionTree::MakeDecision()
{
	//Update();

	auto currentNode = m_treeNodeList[0];

	while (!currentNode->isLeaf)
		currentNode = (currentNode->data) ? (currentNode->Right) : (currentNode->Left);

	setCurrentNode(currentNode);
	return currentNode->name;


}

void DecisionTree::m_buildTree()
{
	switch (m_agent->getAgentType())
	{
	case RANGED_ENEMY:
	{
		m_HealthNode = new HealthCondition(); //ROOT NODE
		m_treeNodeList.push_back(m_HealthNode); //Node 0, Check if Health < 25%

		m_RecentlyHitNode = new RecentlyHitCondition();
		AddNode(m_HealthNode, m_RecentlyHitNode, LEFT_TREE_NODE); //HEALTH NODE LEFT
		m_treeNodeList.push_back(m_RecentlyHitNode); //Node 1, If Health > 25%, Check if Recently Hit

		TreeNode* fleeNode = AddNode(m_HealthNode, new FleeAction, RIGHT_TREE_NODE); //HEALTH NODE RIGHT
		m_treeNodeList.push_back(fleeNode); //Node 2, if Health < 25%, Flee

		m_RadiusNode = new RadiusCondition();
		AddNode(m_RecentlyHitNode, m_RadiusNode, LEFT_TREE_NODE); //RECENTLY HIT LEFT
		m_treeNodeList.push_back(m_RadiusNode); //Node 3, if no LOS, check if in detection Radius

		m_BehindCoverNode = new BehindCoverCondition();
		AddNode(m_RecentlyHitNode, m_BehindCoverNode, RIGHT_TREE_NODE); //RECENTLY HIT RIGHT
		m_treeNodeList.push_back(m_BehindCoverNode); //Node 4, if Recently Hit, Check if Behind Cover

		TreeNode* moveToCoverNode = AddNode(m_BehindCoverNode, new MoveBehindCoverAction, LEFT_TREE_NODE); //BEHIND COVER LEFT 
		m_treeNodeList.push_back(moveToCoverNode); //Node 5, if not Behind Cover, Move to Cover

		TreeNode* waitBehindCoverNode = AddNode(m_BehindCoverNode, new WaitBehindCoverAction, RIGHT_TREE_NODE); //BEHIND COVER RIGHT
		m_treeNodeList.push_back(waitBehindCoverNode); //Node 6, if Behind Cover, Wait specified time

		TreeNode* patrolNode = AddNode(m_RadiusNode, new PatrolAction(), LEFT_TREE_NODE); //RADIUS LEFT
		m_treeNodeList.push_back(patrolNode); //Node 7, if no DR, Patrol Action

		m_LOSNode = new LOSCondition();
		AddNode(m_RadiusNode, m_LOSNode, RIGHT_TREE_NODE); //RADIUS RIGHT
		m_treeNodeList.push_back(m_LOSNode); //Node 8

		TreeNode* moveToLOSNode = AddNode(m_LOSNode, new MoveToLOSAction, LEFT_TREE_NODE); //LOS LEFT
		m_treeNodeList.push_back(moveToLOSNode); //Node 9, if has DR, Move to LOS

		m_AttackRangeNode = new AttackRangeCondition();
		AddNode(m_LOSNode, m_AttackRangeNode, RIGHT_TREE_NODE); //LOS RIGHT
		m_treeNodeList.push_back(m_AttackRangeNode); //Node 10, if LOS, check if in Attack Range

		TreeNode* moveToPlayerNode = AddNode(m_AttackRangeNode, new MoveToPlayerAction, LEFT_TREE_NODE); //ATTACK RANGE LEFT
		m_treeNodeList.push_back(moveToPlayerNode); //Node 11, if not in Attack Range, Move to Player

		m_RangeDistanceNode = new TooCloseCondition(); //ATTACK RANGE RIGHT
		AddNode(m_AttackRangeNode, m_RangeDistanceNode, RIGHT_TREE_NODE); //Node 12, if in Attack Range, Check if too Close

		TreeNode* rangeAttackNode = AddNode(m_RangeDistanceNode, new RangeAttackAction, LEFT_TREE_NODE); //RANGE DISTANCE LEFT
		m_treeNodeList.push_back(rangeAttackNode); //Node 13, if not too close, Range Attack

		TreeNode* moveToARNode = AddNode(m_RangeDistanceNode, new MoveToRangeAction, RIGHT_TREE_NODE); //RANGE DISTANCE RIGHT
		m_treeNodeList.push_back(moveToARNode); //Node 14, if too close, Move to Attack Range Threshold

		break;
	}
	case CLOSE_COMBAT_ENEMY:
	{
		m_HealthNode = new HealthCondition(); //ROOT NODE
		m_treeNodeList.push_back(m_HealthNode); //Node 0, Check if Health < 25%

		m_RadiusNode = new RadiusCondition();
		AddNode(m_HealthNode, m_RadiusNode, LEFT_TREE_NODE); //HEALTH LEFT
		m_treeNodeList.push_back(m_RadiusNode);//Node 1, Check Radius

		TreeNode* fleeCNode = AddNode(m_HealthNode, new FleeAction, RIGHT_TREE_NODE); //HEALTH RIGHT
		m_treeNodeList.push_back(fleeCNode); //Node 2, if Health < 25%, Flee

		TreeNode* patrolCNode = AddNode(m_RadiusNode, new PatrolAction(), LEFT_TREE_NODE); //RADIUS LEFT
		m_treeNodeList.push_back(patrolCNode); //Node 3, if no DR, Patrol Action

		m_LOSNode = new LOSCondition();
		AddNode(m_RadiusNode, m_LOSNode, RIGHT_TREE_NODE); //RADIUS RIGHT
		m_treeNodeList.push_back(m_LOSNode); //Node 4, if Health > 25%, check LOS

		TreeNode* moveToLOSCNode = AddNode(m_LOSNode, new MoveToLOSAction, LEFT_TREE_NODE); //LOS RIGHT
		m_treeNodeList.push_back(moveToLOSCNode); //Node 5, if has DR, Move to LOS

		m_CloseCombatNode = new CloseCombatCondition();
		AddNode(m_LOSNode, m_CloseCombatNode, RIGHT_TREE_NODE); //LOS RIGHT
		m_treeNodeList.push_back(m_CloseCombatNode); //Node 6, if LOS, check if in CC Range

		TreeNode* moveToPlayerCNode = AddNode(m_CloseCombatNode, new MoveToPlayerAction, LEFT_TREE_NODE); //CLOSE COMBAT RANGE LEFT
		m_treeNodeList.push_back(moveToPlayerCNode); //Node 7, if not in Attack Range, Move to Player

		TreeNode* closeCombatAttack = AddNode(m_CloseCombatNode, new CloseCombatAction, RIGHT_TREE_NODE); //CLOSE COMBAT RANGE RIGHT
		m_treeNodeList.push_back(closeCombatAttack); //Node 8
		break;
	}
	case AGENT_PLAYER:
		break;
	}
}
