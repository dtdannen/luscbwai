/*
* Goal Advisor
* Author: Steven Stinson (srs514)
* Intended to determine the next/current goal region based on the data from the Color Graph.
* Call update to update the goal region, and getGoalRegion() if you want to get the current goal region.
*/
#include "ColorGraph.h"
#include "ColorNode.h"

#pragma once

class ColorNode;

class GoalAdvisor {

private:
	int goalNode;
	int originNode;

public:
	int getGoalRegion();
	void update();
	GoalAdvisor();
	~GoalAdvisor();
}
