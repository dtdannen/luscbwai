#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Squad.h"
#include <Arbitrator.h>

//! This class manages the execution of the Slow Push.
/*!

*/
class ChokePriority {
public:
	ChokePriority(BWTA::Chokepoint*, int, ChokePointAdvisor*);
	BWTA::Chokepoint* getChokepoint();
	int getPriority();
	Squad* getSquad();
	void setSquad(Squad* squad);
	ChokePointAdvisor* getAdvisor();

private:
	BWTA::Chokepoint* chokepoint;
	int priority;
	Squad* squad;
	ChokePointAdvisor* advisor;
};
