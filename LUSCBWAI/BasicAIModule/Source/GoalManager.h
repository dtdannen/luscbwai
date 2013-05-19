#pragma once
#include <BWAPI.h>
#include <BuildOrderManager.h>
#include "DebugPrinter.h"
#include <string>

class GoalManager
{
public:
	GoalManager(BuildOrderManager * buildOrderManager);
	~GoalManager(void);

	void newGoal(std::map<BWAPI::UnitType, int> composition);
	void update();
	void refresh(); 
	bool readyToRefresh();


private:
	BuildOrderManager * buildOrderManager;
	std::map<BWAPI::UnitType, int> composition;

	// this keeps track of how many units we are expecting so that we know when we need to refresh
	std::map<BWAPI::UnitType, int> expectedCounts;
};

