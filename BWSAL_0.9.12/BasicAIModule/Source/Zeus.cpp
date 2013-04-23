#include "Zeus.h"

Zeus::Zeus()
{
}

/*
	To be run in the main module's onFrame().
	Consults advisors, decides upon goals, and then communicates them to managers.
*/
void Zeus::update()
{
	//Gather information from advisors
	//Decide upon new goals
	//Send them
	//sendMacroEdict();
	std::map<BWAPI::UnitType, int> goal;
	goal[BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode] = 5;
	sendStrategyEdict(StrategyGoals::SLOW_PUSH);
}

Zeus & Zeus::Instance()
{
	static Zeus instance;
	return instance;
}

/* 
	Send a goal to the macro manager
*/
void Zeus::sendMacroEdict(std::map<BWAPI::UnitType, int> goal)
{
	//will use something like the following:
	macroManager -> newGoal(goal);
}

/*
	Send a goal to the strategy manager
*/
void Zeus::sendStrategyEdict(int goal)
{
	//will use something like the following:
	//stategyManager.setCurrentGoal(goal);
}

/*
	Set the macro manager
*/
void Zeus::setMacroManager(GoalManager * macMan)
{
	macroManager = macMan;
}

//void Zeus::setStrategyManager(StrategyManager stratMan)
//{
//	strategyManager = stratMan;
//}
