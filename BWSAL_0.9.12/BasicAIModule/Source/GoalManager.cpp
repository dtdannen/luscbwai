#include "GoalManager.h"
#include <string>


GoalManager::GoalManager(BuildOrderManager * buildOrderManager)
{
	this->buildOrderManager = buildOrderManager;
}


GoalManager::~GoalManager(void)
{
}

void GoalManager::newGoal(std::map<BWAPI::UnitType, int> composition)
{
	this->buildOrderManager->clear();
	//int count = 0;
	for (std::map<BWAPI::UnitType, int>::const_iterator i = composition.begin(); 
															i != composition.end(); i++)
	{
		DebugPrinter::printDebug("Adding units to BOM");
		this->buildOrderManager->buildAdditional(i->second, i->first, 50);
		//count++;
		//BWAPI::Broodwar->sendText("Just decremented count");
	}
}