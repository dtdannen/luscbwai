#include "SafetyAdvisor.h"


SafetyAdvisor::SafetyAdvisor(void)
{
	unitWeight = 1;
	totalNetWeight = 1;
	visibleWeight = 2;
}


SafetyAdvisor::~SafetyAdvisor(void)
{
}

SafetyAdvisor & SafetyAdvisor::Instance()
{
	static SafetyAdvisor instance;
	return instance;
}

void SafetyAdvisor::countUnits()
{
	ourSupply = 0;
	visibleSupply = 0;
	for each(BWAPI::Unit * u in BWAPI::Broodwar->getAllUnits())
	{
		if (u->getPlayer() == BWAPI::Broodwar->self())
		{
			ourSupply += u->getType().supplyRequired();
		}
		else if (u->getPlayer() == BWAPI::Broodwar->enemy())
		{
			visibleSupply += u->getType().supplyRequired();
		}
	}
}

void SafetyAdvisor::OnUpdate()
{
	countUnits();
	BWAPI::Broodwar->sendText("%f", safetyMeasure());
}

void SafetyAdvisor::OnUnitDestroyed(BWAPI::Unit * u)
{
	if (u->getPlayer() == BWAPI::Broodwar->self())
	{
		totalNetSupply -= u->getType().supplyRequired();
	}
	else if (u->getPlayer() == BWAPI::Broodwar->enemy())
	{
		totalNetSupply += u->getType().supplyRequired();
	}
}

double SafetyAdvisor::safetyMeasure()
{
	return unitWeight * (ourSupply - (visibleWeight * visibleSupply)) + totalNetWeight * totalNetSupply;
}