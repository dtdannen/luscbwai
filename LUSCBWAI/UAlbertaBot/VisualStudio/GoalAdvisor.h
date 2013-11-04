/*
* Goal Advisor
* Author: Steven Stinson (srs514)
* Intended to determine the next/current goal region based on the data from the Color Graph.
* Call update to update the goal region, and getGoalRegion() if you want to get the current goal region.
*/
#include "ColorGraph.h"
#include "ColorNode.h"

#ifndef GOAL_ADVISOR
#define GOAL_ADVISOR

class ColorNode;

class GoalAdvisor {

private:
	ColorNode* goalNode;
	ColorNode* originNode;

public:
	ColorNode* getGoalRegion();
	void update();
	GoalAdvisor();
	~GoalAdvisor();
}

#endif