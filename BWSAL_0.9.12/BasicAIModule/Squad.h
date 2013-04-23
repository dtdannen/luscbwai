#pragma once


#include <BWAPI.h>
#include <BWTA.h>
#include "ChokePointAdvisor.h"

class Squad
{
public:
	//! Represents the behavior of a particular squad.
	//! FLEE means a squad will always run from combat.
	//! STAY means a squad will never move from its position, even if it predicts destruction.
	//! DEFENSIVE means a squad will hold its position as long as it can, fleeing if it predicts 
	//!  destruction.
	//! AGGRESSIVE means a squad will hold its position, chase fleeing units, and kite as it is
	//!  fleeing destruction.
	enum SquadBehavior {DEFENSIVE, AGGRESSIVE, STAY, FLEE};

	//! Constructs an empty squad.
	/*!
		\param unitsPerSquad The unit composition of this squad.
		\param advisor The choke point advisor that this squad will use.
	*/
	Squad(std::map<BWAPI::UnitType*, int> unitsPerSquad, ChokePointAdvisor advisor);

	//! Moves the squad to the given position.
	/*!
		\param pos The position to move the squad to.
	*/
	void moveSquad(BWAPI::Position* pos);

	//! Gets the current position of the squad.
	/*!
		\return The position the squad is holding.
	*/
	BWAPI::Position* getPosition();

	//! Changes the behavior of the squad.
	/*!
		\param behavior The new behavior of the squad.
	*/
	void setBehavior(SquadBehavior behavior);

	//! Adds the given unit to the squad, if there is room.
	/*!
		This method will add the unit to the squad only if the squad has room for a unit of the
		given type.
		\param u The unit to attempt to add to the squad.
		\return True if the unit was added, false otherwise.
	*/
	bool addUnit(BWAPI::Unit* u);

	//! Removes the given unit to the squad, if it is in the squad.
	/*!
		This method will remove the unit from the squad. If the given unit is not in the squad,
		nothing happens.
		\param u The unit to remove from the squad.
	*/
	void removeUnit(BWAPI::Unit* u);

	//! Returns the number of units in the squad.
	/*!
		\return The number of units in the squad.
	*/
	int getNumUnits();

	//! Returns the number of units within operating range of the position.
	/*!
		\return The number of units within attack range of the position if not aggressive. If 
				aggressive, returns the number of units within two times the attack range.
	*/
	int getNumInPosition();

	//! Updates this squad.
	/*!
		Should be called every frame.
	*/
	void update();
private:
	std::map<BWAPI::UnitType*, BWAPI::Unit*> units;
	ChokePointAdvisor chokePointAdvisor;
	SquadBehavior behavior;
	BWAPI::Position* position;
	BWAPI::Unit* attack;

	std::set<BWAPI::Unit*> getDangerousEnemies();
	std::set<BWAPI::Unit*> getTargetableEnemies();
	int totalDPS(std::set<BWAPI::Unit*> units);
	BWAPI::Unit* getWeakestUnit(std::set<BWAPI::Unit*> units);
	void flee();
	bool isInRange(BWAPI::Unit*);
	bool isInRange(BWAPI::Unit* u, int positionModifier);
};

