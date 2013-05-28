#include "ChokePointAdvisor.h"

using namespace BWAPI;

ChokePointAdvisor::ChokePointAdvisor(void)
{
}

ChokePointAdvisor::~ChokePointAdvisor(void)
{
}

void ChokePointAdvisor::update()
{
}

void ChokePointAdvisor::addEnemyBase(BWTA::BaseLocation* base)
{
	// checks to make sure this base is not already in enemyBases
	for(std::set<BWTA::BaseLocation*>::iterator i=enemyBases.begin();i!=enemyBases.end();i++) {
		if((*i)->getGroundDistance(base) == 0) return;
	}

	enemyBases.insert(base);
}

void ChokePointAdvisor::removeEnemyBase(BWTA::BaseLocation* base)
{
	for(std::set<BWTA::BaseLocation*>::iterator i=enemyBases.begin();i!=enemyBases.end();i++) {
		if((*i)->getGroundDistance(base) == 0)
		{
			enemyBases.erase(i);
			i--;
		}
	}
}

int ChokePointAdvisor::pollChoke(BWTA::Chokepoint* choke)
{
	return quickPoll(choke);
}

std::set<BWTA::Chokepoint*> ChokePointAdvisor::contestedChokes()
{
	std::set<BWTA::Chokepoint*> allChokes = BWTA::getChokepoints();
	std::set<BWTA::Chokepoint*> ret;
	for(std::set<BWTA::Chokepoint*>::iterator i=allChokes.begin();i!=allChokes.end();i++) {
		int buff = chokeData((*i));
		if((buff >> 2) & 1) ret.insert((*i));
	}

	return ret;
}

std::set<BWTA::Chokepoint*> ChokePointAdvisor::heldChokes()
{
	std::set<BWTA::Chokepoint*> allChokes = BWTA::getChokepoints();
	std::set<BWTA::Chokepoint*> ret;
	for(std::set<BWTA::Chokepoint*>::iterator i=allChokes.begin();i!=allChokes.end();i++) {
		int buff = chokeData((*i));
		if((buff >> 1) & 1) ret.insert((*i));
	}

	return ret;
}

std::set<BWTA::Chokepoint*> ChokePointAdvisor::controlledChokes()
{
	std::set<BWTA::Chokepoint*> allChokes = BWTA::getChokepoints();
	std::set<BWTA::Chokepoint*> ret;
	for(std::set<BWTA::Chokepoint*>::iterator i=allChokes.begin();i!=allChokes.end();i++) {
		int buff = chokeData((*i));
		if(buff & 1) ret.insert((*i));
	}

	return ret;
}

BWTA::Chokepoint* ChokePointAdvisor::priorityChoke()
{
	std::set<BWTA::Chokepoint*> chokes;

	chokes = contestedChokes();
	if(chokes.empty()) chokes = heldChokes();
	if(chokes.empty()) chokes = controlledChokes();
	if(chokes.empty()) return NULL;

	BWTA::Chokepoint* choke = *(chokes.begin());
	int score = pollChoke(choke);
	double dist = distToEnemy(choke);

	for(std::set<BWTA::Chokepoint*>::iterator i=chokes.begin();i!=chokes.end();i++) {
		int newSc = pollChoke(*i);
		double newD = distToEnemy(*i);
		if(newSc > score)
		{
			choke = *i;
			score = newSc;
			dist = newD;
		}
		else if(newSc == score && newD < dist)
		{
			choke = *i;
			score = newSc;
			dist = newD;
		}
	}

	return choke;
}


int ChokePointAdvisor::quickPoll(BWTA::Chokepoint* choke)
{
	int buffer = chokeData(choke);

	if(buffer >> 2) return 5;
	if((buffer >> 1) & 1) return 3;

	return 0;
}

int ChokePointAdvisor::chokeData(BWTA::Chokepoint* choke)
{
	BWTA::Region* r1 = choke->getRegions().first;
	BWTA::Region* r2 = choke->getRegions().second;

	int enemies1 = 0;
	int enemies2 = 0;

	std::set<Unit*> units = Broodwar->enemy()->getUnits();
	for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++) {
		if(r1->getCenter() == BWTA::getRegion((*i)->getPosition())->getCenter()) enemies1++;
		if(r2->getCenter() == BWTA::getRegion((*i)->getPosition())->getCenter()) enemies2++;
	}

	int meUnits1 = 0;
	int meUnits2 = 0;

	int meBuildings1 = 0;
	int meBuildings2 = 0;

	units = Broodwar->self()->getUnits();
	for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++) {
		if(r1->getCenter() == BWTA::getRegion((*i)->getPosition())->getCenter()) {
			if((*i)->getType().isBuilding()) meBuildings1++;
			else meUnits1++;
		}
		if(r2->getCenter() == BWTA::getRegion((*i)->getPosition())->getCenter()) {
			if((*i)->getType().isBuilding()) meBuildings2++;
			else meUnits2++;
		}
	}

	bool meHasPresence1 = false;
	bool meHasPresence2 = false;
	
	if(meUnits1 >= 4 || meBuildings1 >=2) meHasPresence1 = true;
	if(meUnits2 >= 4 || meBuildings2 >=2) meHasPresence2 = true;

	bool enemyHasPresence1 = (enemies1 > 0);
	bool enemyHasPresence2 = (enemies2 > 0);
	
	bool contested1 = meHasPresence1 && enemyHasPresence1;
	bool contested2 = meHasPresence2 && enemyHasPresence2;
	
	bool empty1 = !meHasPresence1 && !enemyHasPresence1;
	bool empty2 = !meHasPresence2 && !enemyHasPresence2;
	
	bool control1 = meHasPresence1 && !enemyHasPresence1;
	bool control2 = meHasPresence2 && !enemyHasPresence2;

	int buffer = 0;

	if((contested1 && control2) || (contested2 && control1)) buffer += 4; //contested
	if((empty1 && control2) || (empty2 && control1)) buffer += 2; //held
	if(control1 && control2) buffer++; //controlled;
	return buffer;
}

double ChokePointAdvisor::distToEnemy(BWTA::Chokepoint* choke)
{
	double dist = -1;
	if(enemyBases.empty()) return dist;

	for(std::set<BWTA::BaseLocation*>::iterator i=enemyBases.begin();i!=enemyBases.end();i++) {
		double newDist = BWTA::getGroundDistance((*i)->getTilePosition(),TilePosition(choke->getCenter()));
		if(dist == -1) dist = newDist;
		else if(newDist < dist) dist = newDist;
	}

	return dist;
}