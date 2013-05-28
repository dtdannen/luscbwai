#include "SlowPushManager.h"
#include <InformationManager.h>
#include <BorderManager.h>
#include <UnitGroupManager.h>
#include "ChokePointAdvisor.h"
#include "../Addons/Util.h"


SlowPushManager::SlowPushManager(Arbitrator::Arbitrator<BWAPI::Unit*,double> *arbitrator, ChokePointAdvisor *chokePointAdvisor)
{
	this->arbitrator = arbitrator;
	this->chokePointAdvisor = chokePointAdvisor;
	this->chokePrioritySet = std::vector<ChokePriority>();
}

void SlowPushManager::giveGoal(int g) {
	this->goal = g;
}

void SlowPushManager::update() {
	// Bid on all completed military units
	std::set<BWAPI::Unit*> myPlayerUnits=BWAPI::Broodwar->self()->getUnits();
	for (std::set<BWAPI::Unit*>::iterator u = myPlayerUnits.begin(); u != myPlayerUnits.end(); u++)
	{
		if (this->armyUnits.find((*u)) == this->armyUnits.end() && // don't bid if we alreayd have
			(*u)->isCompleted() && 
			!(*u)->getType().isWorker() && 
			!(*u)->getType().isBuilding() &&
			(*u)->getType() != BWAPI::UnitTypes::Zerg_Egg &&
			(*u)->getType() != BWAPI::UnitTypes::Zerg_Larva)
		{
			arbitrator->setBid(this, *u, 20);
			//BWAPI::Broodwar->sendText("SlowPushManager bid on another unit!");
		}
	}

	if (this->armyUnits.size() != 0 && readyForNewCommand() && goal == SLOWPUSH) {
		//BWAPI::Broodwar->sendText("About to call distribute units in SlowPushManager -----------");
		std::set<BWTA::Chokepoint*> chokepoints = getNextChokepoints();
		distributeUnits(chokepoints);
		advanceSlowPush(chokepoints);
	}
}

// bug is here!!
void SlowPushManager::distributeUnits(std::set<BWTA::Chokepoint*> chokepoints) {
	// merge any new chokepoints into the current set of choke priorities
	bool isNewChokepoint = true;
	for (std::set<BWTA::Chokepoint*>::iterator c = chokepoints.begin(); c != chokepoints.end();c++){
		isNewChokepoint = true;
		// check to see if any of the current choke prioities represent this chokepoint
		for (std::vector<ChokePriority>::iterator cp = this->chokePrioritySet.begin(); 
			cp != this->chokePrioritySet.end(); cp++) {
				if ((*c)->getCenter().x() == (*cp).getChokepoint()->getCenter().x() &&
					(*c)->getCenter().y() == (*cp).getChokepoint()->getCenter().y()) {
					// the center of a chokepoint can be used as a unique identifier
					// if they match, this is not new
					isNewChokepoint = false;
				}
			}
		if (isNewChokepoint) {
			// add to the chokepriorities set
			BWAPI::Broodwar->sendText("Added a new choke point to the choke priority set!!!!!");
			this->chokePrioritySet.push_back(ChokePriority((*c),this->chokePointAdvisor->pollChoke((*c)), this->chokePointAdvisor));
		}
	}

	std::set<BWAPI::Unit*> tanks;
	std::set<BWAPI::Unit*> marines;
	std::set<BWAPI::Unit*> vultures;
	double bid = 100;
	int totalPriority = 0;

	// break up units into sets for tanks, marines, and vultures
	for (std::set<BWAPI::Unit*>::iterator i = armyUnits.begin(); i != armyUnits.end(); i++) {
		if ((*i)->getType().getID() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode.getID() ||
			(*i)->getType().getID() == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode.getID()) {
				tanks.insert((*i));
		}else if ((*i)->getType().getID() == BWAPI::UnitTypes::Terran_Marine.getID()) {
			marines.insert((*i));
		}else if ((*i)->getType().getID() == BWAPI::UnitTypes::Terran_Vulture.getID()) {
			vultures.insert((*i));
		}
	}

	// Getting priority of each chokepoint.
	for (std::vector<ChokePriority>::iterator cp = this->chokePrioritySet.begin(); 
			cp != this->chokePrioritySet.end(); cp++) {
		totalPriority += (*cp).getPriority();
	}



	// Dividing units by chokepoint priority.
	// Unit iterators
	std::set<BWAPI::Unit*>::iterator t = tanks.begin();
	//BWAPI::Broodwar->sendText("SlowPushManager has %d tanks", tanks.size());
	std::set<BWAPI::Unit*>::iterator m = marines.begin();
	//BWAPI::Broodwar->sendText("SlowPushManager has %d marines", marines.size());
	std::set<BWAPI::Unit*>::iterator v = vultures.begin();
	//BWAPI::Broodwar->sendText("SlowPushManager has %d vultures", vultures.size());

	//BWAPI::Broodwar->sendText("SlowPushManager has %d chokepriorities", chokePrioritySet.size());

	// Unit counters
	int tc = 0;
	int mc = 0;
	int vc = 0;

	// TO-DO [dtdannen]: may need to check that a unit does not exist in another squad before adding
	// it to a new squad.

	for (std::vector<ChokePriority>::iterator i = chokePrioritySet.begin(); i != chokePrioritySet.end(); i++) {
		// whatever the percentage priority the current chokepoint has compared to the total,
		// assign that percentage of units to that choke point
		for (int u = tc; u != (((*i).getPriority()/totalPriority) * (tanks.size())); u++) {
			if (!(*i).getSquad()->hasUnit(*t)) {
				//BWAPI::Broodwar->sendText("Adding tank to chokePriority", chokePrioritySet.size());
				(*i).getSquad()->addUnit(*t);
				t++;
				tc++;
			}
		}
		for (int u = mc; u != (((*i).getPriority()/totalPriority) * (marines.size())); u++) {
			//BWAPI::Broodwar->sendText("Squad does not have marine with id %d", (*m)->getID());
			if (!(*i).getSquad()->hasUnit(*m)) {
				//BWAPI::Broodwar->sendText("Adding marine with id %d to chokePriority", (*m)->getID());
				(*i).getSquad()->addUnit(*m);
				m++;
				mc++;
			}
		}
		for (int u = vc; u != (((*i).getPriority()/totalPriority) * (vultures.size())); u++) {
			if (!(*i).getSquad()->hasUnit(*v)) {
				//BWAPI::Broodwar->sendText("Adding vulture to chokePriority", chokePrioritySet.size());
				(*i).getSquad()->addUnit(*v);
				v++;
				vc++;
			}
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
	//BWAPI::Broodwar->sendText("Ready for new command returns true, which means all %d squads are in position", chokePrioritySet.size());
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
			//BWAPI::Broodwar->sendText("SlowPushManager received on another unit!");
		}
	}
	this->distributeUnits(getNextChokepoints());
}

void SlowPushManager::onRevoke(BWAPI::Unit* unit, double bid)
{
	armyUnits.erase(unit);
}

void SlowPushManager::onRemoveUnit(BWAPI::Unit* unit)
{
	armyUnits.erase(unit);
}