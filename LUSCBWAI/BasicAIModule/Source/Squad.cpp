#include "Squad.h"


Squad::Squad(std::map<BWAPI::UnitType*, int> unitsPerSquad, ChokePointAdvisor* advisor)
{
	for(std::map<BWAPI::UnitType*, int>::iterator i = unitsPerSquad.begin(); 
												  i != unitsPerSquad.end(); i++)
	{
		for(int num = (*i).second; num > 0; num--)
		{
			std::pair<BWAPI::UnitType*, BWAPI::Unit*> pair((*i).first, NULL);
			this->units.insert(pair);
		}
	}
	chokePointAdvisor = advisor;
	attack = NULL;
}

void Squad::moveSquad(BWAPI::Position* pos)
{
	this->position = pos;
}

BWAPI::Position* Squad::getPosition()
{
	return this->position;
}

void Squad::setBehavior(SquadBehavior behavior)
{
	this->behavior = behavior;
}

bool Squad::addUnit(BWAPI::Unit* u)
{
	BWAPI::UnitType* type = &(u->getType());
	for(std::map<BWAPI::UnitType*,BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
	{
		if((*i).first == type && (*i).second == NULL)
		{
			std::pair<BWAPI::UnitType*, BWAPI::Unit*> pair(type, u);
			units.erase(i);
			units.insert(pair);
			return true;
		}
	}
	return false;
}

void Squad::removeUnit(BWAPI::Unit* u)
{
	for(std::map<BWAPI::UnitType*,BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
	{
		if((*i).second == u)
		{
			std::pair<BWAPI::UnitType*, BWAPI::Unit*> pair(&(u->getType()), NULL);
			units.erase(i);
			units.insert(pair);
		}
	}
}

int Squad::getNumUnits()
{
	int count = 0;
	for(std::map<BWAPI::UnitType*,BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
		if((*i).second != NULL)
			count++;
	return count;
}

void Squad::update()
{
	if(attack != NULL && attack->getHitPoints() <= 0 || behavior == FLEE)
		attack = NULL;

	std::set<BWAPI::Unit*> dangerousEnemies = getDangerousEnemies();
	std::set<BWAPI::Unit*> targetableEnemies = getTargetableEnemies();

	if(behavior == STAY)
	{
		if(attack == NULL)
			attack = getWeakestUnit(targetableEnemies);
	}
	else if(behavior == FLEE)
	{
		if(!dangerousEnemies.empty())
			flee();
	}
	else if(behavior == DEFENSIVE)
	{
		if(totalDPS(dangerousEnemies) > totalDPS(targetableEnemies))
			flee();
		else
			attack = getWeakestUnit(targetableEnemies);
	}
	else if(behavior == AGGRESSIVE)
	{
		if(totalDPS(dangerousEnemies) > totalDPS(targetableEnemies))
			flee();
		else if(attack == NULL)
			attack = getWeakestUnit(targetableEnemies);
	}

	for(std::map<BWAPI::UnitType*,BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
	{
		BWAPI::Unit* unit = (*i).second;
		if(unit == NULL)
			continue;
		bool isWithinPosition = isInRange(unit);
		if(!isWithinPosition && !(*i).second->isMoving())
		{
			unit->move(*position);
			attack = NULL;
		}
		else if(isWithinPosition && attack != NULL && !(*i).second->isAttacking())
		{
			unit->attack(attack);
		}
	}
}

void Squad::flee()
{
	BWTA::Region* region = BWTA::getRegion(*(new BWAPI::TilePosition(*position)));
	std::set<BWTA::Chokepoint*> chokes = region->getChokepoints();
	bool existsSafeChoke = false;
	BWTA::Chokepoint* closestSafeChoke = *chokes.begin();
	for(std::set<BWTA::Chokepoint*>::iterator i = chokes.begin(); i != chokes.end(); i++)
	{
		//"3" appears to mean that we have control.
		if(chokePointAdvisor->pollChoke(*i) == 3 && position->getDistance((*i)->getCenter()) <= position->getDistance((closestSafeChoke->getCenter())))
		{
			existsSafeChoke = true;
			closestSafeChoke = *i;
		}
	}

	if(existsSafeChoke)
		this->moveSquad(&closestSafeChoke->getCenter());
	else
		this->moveSquad(new BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()));
}

BWAPI::Unit* Squad::getWeakestUnit(std::set<BWAPI::Unit*> units)
{
	BWAPI::Unit* weakestEnemy = NULL;
	for(std::set<BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
		if(weakestEnemy == NULL || (*i)->getHitPoints() < weakestEnemy->getHitPoints())
			weakestEnemy = (*i);
	return weakestEnemy;
}

int Squad::totalDPS(std::set<BWAPI::Unit*> units)
{
	int total = 0;
	for(std::set<BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
	{
		BWAPI::WeaponType weapon = (*i)->getType().groundWeapon();
		total += (weapon.damageAmount() / weapon.damageCooldown());
	}
	return total;
}

std::set<BWAPI::Unit*> Squad::getTargetableEnemies()
{
	std::set<BWAPI::Unit*> targetableEnemies;
	for(std::map<BWAPI::UnitType*,BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
	{
		BWAPI::Unit* unit = (*i).second;
		if(unit == NULL)
			continue;
		BWAPI::WeaponType ground = unit->getType().groundWeapon();
		BWAPI::WeaponType air = unit->getType().airWeapon();

		std::set<BWAPI::Unit*> inRangeUnits = unit->getUnitsInWeaponRange(ground);
		std::set<BWAPI::Unit*> inRangeAirUnits = unit->getUnitsInWeaponRange(air);
		inRangeUnits.insert(inRangeAirUnits.begin(), inRangeAirUnits.end());

		for(std::set<BWAPI::Unit*>::iterator i = inRangeUnits.begin(); i != inRangeUnits.end();i++)
		{
			std::set<BWAPI::Unit*> enemyUnits = BWAPI::Broodwar->enemy()->getUnits();
			if(enemyUnits.find(*i) != enemyUnits.end())
				targetableEnemies.insert(*i);
		}
	}

	return targetableEnemies;
}

int Squad::getNumInPosition()
{
	int count = 0;
	for(std::map<BWAPI::UnitType*,BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
	{
		BWAPI::Unit* unit = (*i).second;
		if(unit != NULL && isInRange(unit))
			count++;
	}
	return count;
}

std::set<BWAPI::Unit*> Squad::getDangerousEnemies()
{
	std::set<BWAPI::Unit*> dangerousEnemies;
	std::set<BWAPI::Unit*> allEnemies = BWAPI::Broodwar->enemy()->getUnits();
	for(std::set<BWAPI::Unit*>::iterator i = allEnemies.begin(); i != allEnemies.end(); i++)
	{
		BWAPI::WeaponType ground = (*i)->getType().groundWeapon();
		BWAPI::WeaponType air = (*i)->getType().airWeapon();
		
		std::set<BWAPI::Unit*> inRangeUnits = (*i)->getUnitsInWeaponRange(ground);
		std::set<BWAPI::Unit*> inRangeAirUnits = (*i)->getUnitsInWeaponRange(air);
		inRangeUnits.insert(inRangeAirUnits.begin(), inRangeAirUnits.end());

		for(std::map<BWAPI::UnitType*, BWAPI::Unit*>::iterator s = units.begin(); 
															   s != units.end(); s++)
		{
			if((*s).second != NULL && inRangeUnits.find((*s).second) != inRangeUnits.end())
				dangerousEnemies.insert(*i);
		}
	}

	return dangerousEnemies;
}

bool Squad::isInRange(BWAPI::Unit* u)
{
	if(behavior == AGGRESSIVE)
		return isInRange(u, 2);
	return isInRange(u, 1);
}


bool Squad::isInRange(BWAPI::Unit* u, int positionModifier)
{
	double posDistance = u->getPosition().getDistance(*position) * positionModifier;
	int seekRange = u->getType().seekRange();
	return posDistance < seekRange;
}