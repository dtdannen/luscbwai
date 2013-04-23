#include "ComSatManager.h"

/* 
	To be run onUnitShow in the main module.
	Collects all resource depots and comsat stations.
*/
void ComSatManager::onUnitShow(BWAPI::Unit * unit)
{
	if (unit->getType().isResourceDepot() && unit->getPlayer() == BWAPI::Broodwar->self())
	{
		arbitrator->setBid(this, unit, 80);
	}
	if (unit->getType() == BWAPI::UnitTypes::Terran_Comsat_Station)
	{
		if (stealthDetectors.size() < 1)
		{
			arbitrator->setBid(this, unit, 80);
		}
		else
		{
			arbitrator->setBid(this, unit, 80);
		}
	}
}

/*
	To be run on onFrame() in the main module
	Redistributes comsats in the energy lists if necessary
*/
void ComSatManager::update()
{

	//check to see if anything has energy
	for each (BWAPI::Unit * comSat in noEnergyComs)
	{                                                                                              
		if (comSat->getEnergy() > 50)
		{
			if (stealthDetectors.size() < 1)
			{
				stealthDetectors.insert(comSat);
				noEnergyComs.erase(comSat);
			}
			else
			{
				comSats.insert(comSat);
				noEnergyComs.erase(comSat);
			}
		}
	}

	//check if there are any units in the SD set
	if (stealthDetectors.size() < 1 && comSats.size() > 0)
	{
		stealthDetectors.insert(*(comSats.begin()));
		comSats.erase(*(comSats.begin()));
	}
}

/* 
	Requests a scanner sweep to check for stealthed units.
	This is a high priority action, and should only be used when absolutely necessary.
*/
bool ComSatManager::requestDetectionSweep(BWAPI::Position position)
{
	if (stealthDetectors.size() > 0)
	{
		BWAPI::Unit * comSat = (*(stealthDetectors.begin()));
		if (comSat->useTech(BWAPI::TechTypes::Scanner_Sweep, position))
		{
			if (comSat->getEnergy() < 50)
			{
				stealthDetectors.erase(comSat);
				noEnergyComs.insert(comSat);
			}
			return true;
		}
	}
	return false;
}

/*
	Runs a normal priority scanner sweep.
	This should be used for scouting and map control.
*/
bool ComSatManager::requestSweep(BWAPI::Position position)
{
	if (comSats.size() > 0)
	{
		BWAPI::Unit * comSat = (*(comSats.begin()));
		if (comSat->useTech(BWAPI::TechTypes::Scanner_Sweep, position))
		{
			if (comSat->getEnergy() < 50)
			{
				comSats.erase(comSat);
				noEnergyComs.insert(comSat);
			}
			return true;
		}
	}
	return false;
}

ComSatManager & ComSatManager::Instance()
{
	static ComSatManager instance;
	return instance;
}

void ComSatManager::onOffer(std::set<BWAPI::Unit*> units)
{
	for each (BWAPI::Unit * unit in units)
	{
		BWAPI::Broodwar->sendText("Bid accepted");
		arbitrator->accept(this, unit);
		if (unit->getType().isResourceDepot() && unit->getPlayer() == BWAPI::Broodwar->self())
		{
			bases.insert(unit);
		}
		if (unit->getType() == BWAPI::UnitTypes::Terran_Comsat_Station)
		{
			if (stealthDetectors.size() < 1)
			{
				stealthDetectors.insert(unit);
			}
			else
			{
				comSats.insert(unit);
			}
		}
	}
}
	
void ComSatManager::onRevoke(BWAPI::Unit* unit, double bid)
{
	//scout = NULL;
	bases.erase(unit);
	stealthDetectors.erase(unit);
	comSats.erase(unit);
}

std::string ComSatManager::getName() const
{
	return "ComSat Manager";
}