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

private:
	BuildOrderManager * buildOrderManager;
};

