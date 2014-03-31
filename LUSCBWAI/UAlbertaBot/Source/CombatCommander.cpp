#include "Common.h"
#include "CombatCommander.h"

CombatCommander::CombatCommander() 
	: attacking(false)
	, foundEnemy(false)
	, attackSent(false) 
{
	goalRegion = -1;
}

bool CombatCommander::squadUpdateFrame()
{
	return BWAPI::Broodwar->getFrameCount() % 24 == 0;
}

void CombatCommander::update(std::set<BWAPI::Unit *> unitsToAssign)
{
	if(squadUpdateFrame())
	{
		// clear all squad data
		squadData.clearSquadData();

		// give back combat workers to worker manager
		WorkerManager::Instance().finishedWithCombatWorkers();

		// if there are no units to assign, there's nothing to do
		if (unitsToAssign.empty()) { return; }

		// Assign defense and attack squads
		assignDefenseSquads(unitsToAssign);
		assignAttackSquads(unitsToAssign);

		//assignIdleSquads(unitsToAssign);
	}

	squadData.update();
}

void CombatCommander::assignIdleSquads(std::set<BWAPI::Unit *> & unitsToAssign)
{
	if (unitsToAssign.empty()) { return; }

	UnitVector combatUnits(unitsToAssign.begin(), unitsToAssign.end());
	unitsToAssign.clear();

	squadData.addSquad(Squad(combatUnits, SquadOrder(SquadOrder::Defend, BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()), 1000, "Defend Idle")));
}

void CombatCommander::assignAttackSquads(std::set<BWAPI::Unit *> & unitsToAssign)
{
	if (unitsToAssign.empty()) { return; }
	FrontierAdvisor::Instance().recalculateFrontier();
	int goal = FrontierAdvisor::Instance().getNextNodeId(); //GoalAdvisor::Instance().getGoalRegion();
	if (goal != goalRegion)
	{
		UnitVector units(unitsToAssign.begin(), unitsToAssign.end());

		UnitVector tanks;
		UnitVector vultures;
		UnitVector goliaths;
		UnitVector marines;

		int tankCount = 0;
		int vultureCount = 0;
		int goliathCount = 0;
		int marineCount = 0;

		std::vector<Squad> tankSquads;
		std::vector<Squad> vultureSquads;
		std::vector<Squad> goliathSquads;
		std::vector<Squad> marineSquads;

		for each (BWAPI::Unit * unit in units)
		{
			if (unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode || unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode)
			{
				tanks.push_back(unit);
				unitsToAssign.erase(unit);
			}
			else if (unit->getType() == BWAPI::UnitTypes::Terran_Vulture)
			{
				vultures.push_back(unit);
				unitsToAssign.erase(unit);
			}
			else if (unit->getType() == BWAPI::UnitTypes::Terran_Goliath)
			{
				goliaths.push_back(unit);
				unitsToAssign.erase(unit);
			}				
			else if (unit->getType() == BWAPI::UnitTypes::Terran_Marine)
			{
				marines.push_back(unit);
				unitsToAssign.erase(unit);
			}
		}

		UnitVector temp;
		int count = 0;
		for each (BWAPI::Unit * unit in tanks)
		{
			if (count < 6)
			{
				temp.push_back(unit);
				count++;
			}
			else
			{
				count = 0;
				Squad s = Squad(temp, SquadOrder(SquadOrder::Tanks,	ColorGraph::Instance().getNodeCenter(goal), 1000,  "Tanks"));
				squadData.addSquad(s);
				tankSquads.push_back(s);
				temp.clear();
			}
		}

		if (count != 0)
		{
			Squad s = Squad(temp, SquadOrder(SquadOrder::Tanks,	ColorGraph::Instance().getNodeCenter(goal), 1000,  "Tanks"));
			squadData.addSquad(s);
			tankSquads.push_back(s);
		}

		count = 0;
		temp.clear();

		for each (BWAPI::Unit * unit in vultures)
		{
			if (count < 6)
			{
				temp.push_back(unit);
				count++;
			}
			else
			{
				count = 0;
				Squad s = Squad(temp, SquadOrder(SquadOrder::Vultures, ColorGraph::Instance().getNodeCenter(goal), 1000,  "Vultures"));
				squadData.addSquad(s);
				vultureSquads.push_back(s);
				temp.clear();
			}
		}

		if (count != 0)
		{
			Squad s = Squad(temp, SquadOrder(SquadOrder::Vultures, ColorGraph::Instance().getNodeCenter(goal), 1000,  "Vultures"));
			squadData.addSquad(s);
			vultureSquads.push_back(s);
		}

		count = 0;
		temp.clear();

		for each (BWAPI::Unit * unit in goliaths)
		{
			if (count < 6)
			{
				temp.push_back(unit);
				count++;
			}
			else
			{
				count = 0;
				Squad s = Squad(temp, SquadOrder(SquadOrder::Goliaths, ColorGraph::Instance().getNodeCenter(goal), 1000,  "Goliaths"));
				squadData.addSquad(s);
				goliathSquads.push_back(s);
				temp.clear();
			}
		}

		if (count != 0)
		{
			Squad s = Squad(temp, SquadOrder(SquadOrder::Goliaths, ColorGraph::Instance().getNodeCenter(goal), 1000,  "Goliaths"));
			squadData.addSquad(s);
			goliathSquads.push_back(s);
		}

		count = 0;
		temp.clear();

		for each (BWAPI::Unit * unit in marines)
		{
			if (count < 6)
			{
				temp.push_back(unit);
				count++;
			}
			else
			{
				count = 0;
				Squad s = Squad(temp, SquadOrder(SquadOrder::Marines, ColorGraph::Instance().getNodeCenter(goal), 1000,  "Marines"));
				squadData.addSquad(s);
				marineSquads.push_back(s);
				temp.clear();
			}
		}

		if (count != 0)
		{
			Squad s = Squad(temp, SquadOrder(SquadOrder::Marines, ColorGraph::Instance().getNodeCenter(goal), 1000,  "Marines"));
			squadData.addSquad(s);
			marineSquads.push_back(s);
		}

		//squadData.addSquad(Squad(units, SquadOrder(SquadOrder::Attack,  
		//	ColorGraph::Instance().getNodeCenter(goal), 1000,  "Move Test")));

		// update the squad goals to match the frontier importance ratios
		std::set<int> frontierNodes = FrontierAdvisor::Instance().getFrontier();

		// find the importance ratio of the nodes in the frontier
		double sum = 0;
		std::map<int, double> nodeImportance;

		for each(int id in frontierNodes)
		{
			nodeImportance[id] = ColorGraph::Instance().getNodeImportance(id);
			sum += nodeImportance[id];
		}

		for each (int id in frontierNodes)
		{
			nodeImportance[id] /= sum;
		}

		std::map<int, int> tankMap;
		std::map<int, int> vultureMap;
		std::map<int, int> goliathMap;
		std::map<int, int> marineMap;

		// this leaves one squad of each type floating, so we'll add it later
		for each (int id in frontierNodes)
		{
			tankMap[id] = (int)(tankSquads.size() * nodeImportance[id]);
			vultureMap[id] = (int)(vultureSquads.size() * nodeImportance[id]);
			goliathMap[id] = (int)(goliathSquads.size() * nodeImportance[id]);
			marineMap[id] = (int)(marineSquads.size() * nodeImportance[id]);
		}

		int i = 0;
		double max = 0;

		for each (int id in frontierNodes)
		{
			if (nodeImportance[id] > max)
			{
				max = nodeImportance[id];
				i = id;
			}
		}

		if (frontierNodes.size() > 1)
		{
			if (tankSquads.size() > 0)
			{
				tankMap[i]++;
			}
			if (vultureSquads.size() > 0)
			{
				vultureMap[i]++;
			}
			if (goliathSquads.size() > 0)
			{
				goliathMap[i]++;
			}
			if (marineSquads.size() > 0)
			{
				marineMap[i]++;
			}
		}

		for each (int id in frontierNodes)
		{
			while (tankMap[id] > 0)
			{
				tankSquads.front().setSquadOrder(SquadOrder(SquadOrder::Tanks,	ColorGraph::Instance().getNodeCenter(id), 1000,  "Tanks"));
				tankSquads.erase(tankSquads.begin());
				tankMap[id]--;
			}

			while (vultureMap[id] > 0)
			{
				vultureSquads.front().setSquadOrder(SquadOrder(SquadOrder::Vultures,	ColorGraph::Instance().getNodeCenter(id), 1000,  "Vultures"));
				vultureSquads.erase(vultureSquads.begin());
				vultureMap[id]--;
			}

			while (goliathMap[id] > 0)
			{
				goliathSquads.front().setSquadOrder(SquadOrder(SquadOrder::Goliaths,	ColorGraph::Instance().getNodeCenter(id), 1000,  "Goliaths"));
				goliathSquads.erase(goliathSquads.begin());
				goliathMap[id]--;
			}

			while (marineMap[id] > 0)
			{
				marineSquads.front().setSquadOrder(SquadOrder(SquadOrder::Marines,	ColorGraph::Instance().getNodeCenter(id), 1000,  "Marines"));
				marineSquads.erase(marineSquads.begin());
				marineMap[id]--;
			}
		}
	}

	//bool workersDefending = false;
	//BOOST_FOREACH (BWAPI::Unit * unit, unitsToAssign)
	//{
	//	if (unit->getType().isWorker())
	//	{
	//		workersDefending = true;
	//	}
	//}

	//// do we have workers in combat
	//bool attackEnemy = !unitsToAssign.empty() && !workersDefending && StrategyManager::Instance().doAttack(unitsToAssign);

	//// if we are attacking, what area are we attacking?
	//if (attackEnemy) 
	//{	
	//	//assignAttackRegion(unitsToAssign);				// attack occupied enemy region
	//	//assignAttackKnownBuildings(unitsToAssign);		// attack known enemy buildings
	//	//assignAttackVisibleUnits(unitsToAssign);			// attack visible enemy units
	//	//assignAttackExplore(unitsToAssign);				// attack and explore for unknown units

	//	
	//} 
}

BWTA::Region * CombatCommander::getClosestEnemyRegion()
{
	BWTA::Region * closestEnemyRegion = NULL;
	double closestDistance = 100000;

	// for each region that our opponent occupies
	BOOST_FOREACH (BWTA::Region * region, InformationManager::Instance().getOccupiedRegions(BWAPI::Broodwar->enemy()))
	{
		double distance = region->getCenter().getDistance(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()));

		if (!closestEnemyRegion || distance < closestDistance)
		{
			closestDistance = distance;
			closestEnemyRegion = region;
		}
	}

	return closestEnemyRegion;
}

void CombatCommander::assignDefenseSquads(std::set<BWAPI::Unit *> & unitsToAssign) 
{
	if (unitsToAssign.empty()) { return; }

	// for each of our occupied regions
	BOOST_FOREACH(BWTA::Region * myRegion, InformationManager::Instance().getOccupiedRegions(BWAPI::Broodwar->self()))
	{
		BWAPI::Position regionCenter = myRegion->getCenter();
		if (!regionCenter.isValid())
		{
			continue;
		}

		// start off assuming all enemy units in region are just workers
		int numDefendersPerEnemyUnit = 1;

		// all of the enemy units in this region
		std::set<BWAPI::Unit *> enemyUnitsInRegion;
		BOOST_FOREACH (BWAPI::Unit * enemyUnit, BWAPI::Broodwar->enemy()->getUnits())
		{			
			if (BWTA::getRegion(BWAPI::TilePosition(enemyUnit->getPosition())) == myRegion)
			{
				enemyUnitsInRegion.insert(enemyUnit);

				// if the enemy isn't a worker, increase the amount of defenders for it
				if (!enemyUnit->getType().isWorker())
				{
					numDefendersPerEnemyUnit = 3;
				}
			}
		}

		// figure out how many units we need on defense
		const int numFlyingNeeded = numDefendersPerEnemyUnit * InformationManager::Instance().numEnemyFlyingUnitsInRegion(myRegion);
		const int numGroundNeeded = numDefendersPerEnemyUnit * InformationManager::Instance().numEnemyUnitsInRegion(myRegion);

		if(numGroundNeeded > 0 || numFlyingNeeded > 0)
		{
			// our defenders
			std::set<BWAPI::Unit *> flyingDefenders;
			std::set<BWAPI::Unit *> groundDefenders;

			BOOST_FOREACH (BWAPI::Unit * unit, unitsToAssign)
			{
				if (unit->getType().airWeapon() != BWAPI::WeaponTypes::None)
				{
					flyingDefenders.insert(unit);
				}
				else if (unit->getType().groundWeapon() != BWAPI::WeaponTypes::None)
				{
					groundDefenders.insert(unit);
				}
			}

			// the defense force we want to send
			UnitVector defenseForce;

			// get flying defenders
			for (int i=0; i<numFlyingNeeded && !flyingDefenders.empty(); ++i)
			{
				BWAPI::Unit * flyingDefender = findClosestDefender(enemyUnitsInRegion, flyingDefenders);
				defenseForce.push_back(flyingDefender);
				unitsToAssign.erase(flyingDefender);
				flyingDefenders.erase(flyingDefender);
			}

			// get ground defenders
			for (int i=0; i<numGroundNeeded && !groundDefenders.empty(); ++i)
			{
				BWAPI::Unit * groundDefender = findClosestDefender(enemyUnitsInRegion, groundDefenders);

				if (groundDefender->getType().isWorker())
				{
					WorkerManager::Instance().setCombatWorker(groundDefender);
				}

				defenseForce.push_back(groundDefender);
				unitsToAssign.erase(groundDefender);
				groundDefenders.erase(groundDefender);
			}

			// if we need a defense force, make the squad and give the order
			if (!defenseForce.empty()) 
			{
				squadData.addSquad(Squad(defenseForce, SquadOrder(SquadOrder::Defend, regionCenter, 1000, "Defend Region")));
				return;
			}
		}
	}
}

void CombatCommander::assignAttackRegion(std::set<BWAPI::Unit *> & unitsToAssign) 
{
	if (unitsToAssign.empty()) { return; }

	BWTA::Region * enemyRegion = getClosestEnemyRegion();

	if (enemyRegion && enemyRegion->getCenter().isValid()) 
	{
		UnitVector oppUnitsInArea, ourUnitsInArea;
		MapGrid::Instance().GetUnits(oppUnitsInArea, enemyRegion->getCenter(), 800, false, true);
		MapGrid::Instance().GetUnits(ourUnitsInArea, enemyRegion->getCenter(), 200, true, false);

		if (!oppUnitsInArea.empty())
		{
			UnitVector combatUnits(unitsToAssign.begin(), unitsToAssign.end());
			unitsToAssign.clear();

			squadData.addSquad(Squad(combatUnits, SquadOrder(SquadOrder::Attack, enemyRegion->getCenter(), 1000, "Attack Region")));
		}
	}
}

void CombatCommander::assignAttackVisibleUnits(std::set<BWAPI::Unit *> & unitsToAssign) 
{
	if (unitsToAssign.empty()) { return; }

	BOOST_FOREACH (BWAPI::Unit * unit, BWAPI::Broodwar->enemy()->getUnits())
	{
		if (unit->isVisible())
		{
			UnitVector combatUnits(unitsToAssign.begin(), unitsToAssign.end());
			unitsToAssign.clear();

			squadData.addSquad(Squad(combatUnits, SquadOrder(SquadOrder::Attack, unit->getPosition(), 1000, "Attack Visible")));

			return;
		}
	}
}

void CombatCommander::assignAttackKnownBuildings(std::set<BWAPI::Unit *> & unitsToAssign) 
{
	if (unitsToAssign.empty()) { return; }

	FOR_EACH_UIMAP_CONST (iter, InformationManager::Instance().getUnitInfo(BWAPI::Broodwar->enemy()))
	{
		const UnitInfo ui(iter->second);
		if(ui.type.isBuilding())
		{
			UnitVector combatUnits(unitsToAssign.begin(), unitsToAssign.end());
			unitsToAssign.clear();

			squadData.addSquad(Squad(combatUnits, SquadOrder(SquadOrder::Attack, ui.lastPosition, 1000, "Attack Known")));
			return;	
		}
	}
}

void CombatCommander::assignAttackExplore(std::set<BWAPI::Unit *> & unitsToAssign) 
{
	if (unitsToAssign.empty()) { return; }

	UnitVector combatUnits(unitsToAssign.begin(), unitsToAssign.end());
	unitsToAssign.clear();

	squadData.addSquad(Squad(combatUnits, SquadOrder(SquadOrder::Attack, MapGrid::Instance().getLeastExplored(), 1000, "Attack Explore")));
}

BWAPI::Unit* CombatCommander::findClosestDefender(std::set<BWAPI::Unit *> & enemyUnitsInRegion, const std::set<BWAPI::Unit *> & units) 
{
	BWAPI::Unit * closestUnit = NULL;
	double minDistance = 1000000;

	BOOST_FOREACH (BWAPI::Unit * enemyUnit, enemyUnitsInRegion) 
	{
		BOOST_FOREACH (BWAPI::Unit * unit, units)
		{
			double dist = unit->getDistance(enemyUnit);
			if (!closestUnit || dist < minDistance) 
			{
				closestUnit = unit;
				minDistance = dist;
			}
		}
	}

	return closestUnit;
}

BWAPI::Position CombatCommander::getDefendLocation()
{
	return BWTA::getRegion(BWTA::getStartLocation(BWAPI::Broodwar->self())->getTilePosition())->getCenter();
}

void CombatCommander::drawSquadInformation(int x, int y)
{
	squadData.drawSquadInformation(x, y);
}