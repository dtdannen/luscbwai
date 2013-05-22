#include "SlowPushManager.h"
#include <InformationManager.h>
#include <BorderManager.h>
#include <UnitGroupManager.h>
#include "ChokePointAdvisor.h"



SlowPushManager::SlowPushManager(Arbitrator::Arbitrator<BWAPI::Unit*,double> *arbitrator, ChokePointAdvisor *chokePointAdvisor)
{
	this->arbitrator = arbitrator;
	this->chokePointAdvisor = chokePointAdvisor;
}

void SlowPushManager::giveGoal(int g) {
	this->goal = g;
}

void SlowPushManager::update() {
	if (readyForNewCommand() && goal == SLOWPUSH) {
		std::set<BWTA::Chokepoint*> chokepoints = getNextChokepoints();
		distributeUnits(chokepoints);
		advanceSlowPush(chokepoints);
	}
}

// bug is here!!
void SlowPushManager::distributeUnits(std::set<BWTA::Chokepoint*> chokepoints) {
	std::vector<ChokePriority> chokePrioritySet = std::vector<ChokePriority>();
	std::set<BWAPI::Unit*> tanks;
	std::set<BWAPI::Unit*> marines;
	std::set<BWAPI::Unit*> vultures;
	double bid = 100;
	int totalPriority = 0;
	
	// Getting all attack units.
	tanks = SelectAll()(Siege_Tank)(isCompleted);
	marines = SelectAll()(Marine)(isCompleted);
	vultures = SelectAll()(Vulture)(isCompleted);
	arbitrator->setBid(this, tanks, bid); // Got this from scoutManger where it works?
	arbitrator->setBid(this, marines, bid);
	arbitrator->setBid(this, vultures, bid);
	
	// Getting priority of each chokepoint.
	for (std::set<BWTA::Chokepoint*>::iterator i = chokepoints.begin(); i != chokepoints.end(); i++) {
		ChokePriority choke = ChokePriority(*i, chokePointAdvisor->pollChoke(*i), chokePointAdvisor);

		// this is where its failing
		chokePrioritySet.push_back(choke);
			

		totalPriority += choke.getPriority();
	}
	
	// Dividing units by chokepoint priority.
	// Unit iterators
	std::set<BWAPI::Unit*>::iterator t = tanks.begin();
	std::set<BWAPI::Unit*>::iterator m = marines.begin();
	std::set<BWAPI::Unit*>::iterator v = vultures.begin();
	// Unit counters
	int tc = 0;
	int mc = 0;
	int vc = 0;
	for (std::vector<ChokePriority>::iterator i = chokePrioritySet.begin(); i != chokePrioritySet.end(); i++) {
		for (int u = tc; u != (((*i).getPriority()/totalPriority) * (tanks.size())); u++) {
			(*i).getSquad()->addUnit(*t);
			t++;
			tc++;
		}
		for (int u = mc; u != (((*i).getPriority()/totalPriority) * (marines.size())); u++) {
			(*i).getSquad()->addUnit(*m);
			m++;
			mc++;
		}
		for (int u = vc; u != (((*i).getPriority()/totalPriority) * (vultures.size())); u++) {
			(*i).getSquad()->addUnit(*v);
			v++;
			vc++;
		}
	}

	squads = chokePrioritySet;
	
}

bool SlowPushManager::readyForNewCommand() {
	// for all squads, check to see if all units are in position
	for (std::vector<ChokePriority>::iterator i = squads.begin(); i != squads.end(); i++) {
		if (i->getSquad()->getNumInPosition() != i->getSquad()->getNumUnits()) 
			// this squad does not have all its units in position
			return false;
	}
	return true;
}

void SlowPushManager::advanceSlowPush(std::set<BWTA::Chokepoint*> nextChokepoints) {
	for (std::vector<ChokePriority>::iterator i = squads.begin(); i != squads.end(); i++) {
		BWAPI::Position pos = (i->getChokepoint()->getCenter());
		BWAPI::Position* newPos = new BWAPI::Position(pos.x(), pos.y());
			
			
		(*i).getSquad()->moveSquad(newPos);
	}
}

std::set<BWTA::Chokepoint*> SlowPushManager::getNextChokepoints() {
	std::set<BWTA::Chokepoint*> chokepoints;
	std::set<BWTA::Chokepoint*> contested = chokePointAdvisor->contestedChokes();
	std::set<BWTA::Chokepoint*> held = chokePointAdvisor->heldChokes();

	// Adding all of the contested chokepoints.
	for (std::set<BWTA::Chokepoint*>::iterator i = contested.begin(); i != contested.end(); i++) {
		chokepoints.insert(*i);
	}

	// Adding all of the held chokepoints.
	for (std::set<BWTA::Chokepoint*>::iterator i = held.begin(); i != held.end(); i++) {
		chokepoints.insert(*i);
	}

	// Removing unwanted chokepoints.
	// for (std::set<BWTA::Chokepoint*>::iterator i = chokepoints.begin(); i != chokepoints.end(); i++) { }

	return chokepoints;
}

std::string SlowPushManager::getName() const
{
  return "Slow Push Manager";
}

std::string SlowPushManager::getShortName() const
{
  return "SlwPsh";
}

// adds new army units from arbitrator to the manager's units
void SlowPushManager::onOffer(std::set<BWAPI::Unit*> units)
{
  for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
  {
	  if (armyUnits.find(*u) == armyUnits.end())
    {
      arbitrator->accept(this, *u);
	  armyUnits.insert(*u);
    }
  }
}

void SlowPushManager::onRevoke(BWAPI::Unit* unit, double bid)
{
	armyUnits.erase(unit);
}

void SlowPushManager::onRemoveUnit(BWAPI::Unit* unit)
{
	armyUnits.erase(unit);
}