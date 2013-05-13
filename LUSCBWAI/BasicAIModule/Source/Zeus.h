#pragma once
#include "Common.h"
#include "StrategyGoal.h"
#include "GoalManager.h"
//#include "StrategyManager.h"

class Zeus
{
public:
	void update();
	static Zeus & Instance();

	void setMacroManager(GoalManager * macMan);
	//void setStrategyManager(StrategyManager * stratMan);


private:
	Zeus();
	void sendMacroEdict(std::map<BWAPI::UnitType, int> goal);
	void sendStrategyEdict(int goal);

	StrategyGoals currentStrategyGoal;
	std::map<BWAPI::UnitType, int> currentMacroGoal;

	GoalManager * macroManager;
	//StrategyManager strategyManager;
};

