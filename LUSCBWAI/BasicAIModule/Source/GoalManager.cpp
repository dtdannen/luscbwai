#include "GoalManager.h"
#include <string>

/* Authors: Dustin Dannenhauer
Pete Biencourt

Description: This class is ensures that we are constantly building the proper ratios of the
units we are building.
*/


GoalManager::GoalManager(BuildOrderManager * buildOrderManager)
{
	this->buildOrderManager = buildOrderManager;
	bool finishedLastBuild = false;
}


GoalManager::~GoalManager(void)
{
}

// Given of map specifying how many of each type of unit to produce,
// issues build orders to the build order manager. This should only be called
// to give a new goal (to change the ratio) not to increase or refresh
void GoalManager::newGoal(std::map<BWAPI::UnitType, int> composition)
{
	this->composition = composition;
	this->buildOrderManager->clear();
	//int count = 0;
	for (std::map<BWAPI::UnitType, int>::const_iterator i = composition.begin(); 
		i != composition.end(); i++)
	{
		DebugPrinter::printDebug("Adding units to BOM");
		this->buildOrderManager->buildAdditional(i->second, i->first, 50);
		this->expectedCounts[i->first] += i->second; 
		//count++;
		//BWAPI::Broodwar->sendText("Just decremented count");
	}
}

bool GoalManager::readyToRefresh() {
	for (std::map<BWAPI::UnitType, int>::const_iterator i = composition.begin(); 
		i != composition.end(); i++)
	{
		//BWAPI::Broodwar->sendText("BOM gPC at %d while ePC at %d", this->buildOrderManager->getPlannedCount(i->first),this->expectedCounts[i->first]);
		if (this->buildOrderManager->getPlannedCount(i->first) != this->expectedCounts[i->first]) {
			// if we are still waiting on some units, return false
			return false;
		}
	}
	return true;
}

// refresh goal is just like new goal except it doesn't clear the buildOrderManager,
// this is important because other build orders (i.e. for buildings or workers) will be
// deleted. This is just to build more of the same ratio
void GoalManager::refresh() {
		for (std::map<BWAPI::UnitType, int>::const_iterator i = composition.begin(); 
			i != composition.end(); i++)
		{
			DebugPrinter::printDebug("Adding units to BOM");
			this->buildOrderManager->buildAdditional(i->second, i->first, 50);
		}
}


void GoalManager::update() {
	if (readyToRefresh()) {
		BWAPI::Broodwar->sendText("Ready to refresh");
		refresh();
	}
}
