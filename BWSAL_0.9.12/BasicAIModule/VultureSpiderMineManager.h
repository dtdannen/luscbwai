#pragma once

#include <Arbitrator.h>
#include <BWAPI.h>

//! Manages vultures and where they place their spider mines.
/*!
	This class keeps track of mines which have been placed on the field, locations where mine
	placement is desirable, and the number of mines every vulture on the field still has. As a
	manager, it will request control of vultures that have mines, intelligently move vultures it 
	has control of to place mines on the desirable locations efficiently, and revoke control of 
	vultures that have no mines.
*/
class VultureSpiderMineManager : public Arbitrator::Controller<BWAPI::Unit*,double>
{
public:
	//! The only constructor, to be called at the beginning of the game.
	/*!
		\param arbitrator The arbitrator of the AIModule.
	*/
	VultureSpiderMineManager(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator);

	//! Adds a location to place a mine.
	/*!
		Mines are not necessarily placed in priority order. Please see the documentation for 
		update() for more details.
		\param position The location where a mine is desired.
		\param priority The priority of placing a mine there.
		\sa update()
	*/
	void addDesiredMineLocation(BWAPI::Position* position, int priority);
	
	//! Adds a set of locations to place mines .
	/*!
		Mines are not necessarily placed in priority order. Please see the documentation for 
		update() for more details.
		\param positions The set of locations to place mines.
		\param priority The priority of the set of mines.
		\sa addDesiredMineLocation()
		\sa update()
	*/
	void addDesiredMineLocations(std::set<BWAPI::Position*> positions, int priority);

	//! Returns the set of mines on the field.
	/*!
		\return The set of mines.
	*/
	std::set<BWAPI::Unit*> getMines();

	//! Removes knowledge of the given unit, if relevant.
	/*!
		To be called by the AIModule's onDestroyUnit(Unit* u) function.
		\param u The unit that has been destroyed.
	*/
	void destroy(BWAPI::Unit* u);

	//! Adds knowledge of the given unit, if relevant.
	/*!
		To be called by the AIModule's onUnitCreate(Unit* u) function.
		\param u The unit that has been created.
	*/
	void create(BWAPI::Unit* u);

	//! Gives control of the set of units that are vultures to this manager.
	/*!
		\param units The set of units that this manager is being given control of.
	*/
	virtual void onOffer(std::set<BWAPI::Unit*> units);

	//! Removes control of a unit controlled by this manager.
	/*!
		\param unit The unit being removed.
		\param bid The bid with which it was removed. Ignored by this manager.
	*/
    virtual void onRevoke(BWAPI::Unit* unit, double bid);

	//! Update the manager.
	/*!
		To be called by the AIModule's onFrame() function, each frame.
		This function uses a custom clustering heuristic to send vultures to place mines. First,
		the vulture closest to the highest priority position is chosen; then, it is ordered to
		place mines at that highest priority position and the two positions closest to that,
		regardless of their priority. This means the second-highest priority position is not
		guaranteed to get a mine placed until a second vulture is chosen.
	*/
    virtual void update();

	//! The human readable name of this manager.
    virtual std::string getName() const;

	//! A shortened version of the human readable name of this manager.
	/*!
		\sa getName()
	*/
    virtual std::string getShortName() const;

	//! The arbitrator for the AIModule that this manager is a part of.
	Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;

	//! Each instance of this class keeps track of a vulture's state.
	/*!
		The class keeps track of a single vulture, whether this manager has control of it, 
		its number of mines, the positions it's en route to, and other factors that assist the
		VultureMineManager's orders to the vulture each frame.
	*/
	class VultureMineCount
	{
	public:
		//! The vulture that represents an instance of this class.
		BWAPI::Unit* vulture;

		//! The number of mines that this vulture has to place.
		int numMines;

		//! True if the VultureMineManager has been given control of this vulture, false otherwise.
		bool controls;

		//! The ordered list of locations that this vulture has been given to place mines.
		std::list<BWAPI::Position*> enRoute;

		//! Is 0 if this vulture is not placing a mine, greater than 0 otherwise.
		/*!
			If a vulture is order to place a spider mine, and is given a new order too soon,
			the new order interrupts the previous one and the mine is not placed. This variable 
			solves that problem. When the vulture is ordered to place a mine, this variable is set
			to a constant number of frames. Each frame it is decremented by 1. The vulture will not
			receive any new orders until it is back to 0.
		*/
		int placingMine;

		//! Is true if the vulture is currently moving to a location stored in enRoute, but is not
		//! within range to place a mine.
		bool isMoving;

		//! Constructs a new VultureMineCount.
		/*!
			To be called by VultureMineManager::create(), when the vulture is created.
			\param v The vulture that this instance will represent.
		*/
		VultureMineCount(BWAPI::Unit* v)
		{
			this->vulture = v;
			numMines = 3;
			controls = false;
			placingMine = 0;
			isMoving = false;
		}

		//! Tests if the vulture is available for orders.
		/*!
			\return True if the VultureMineManager has control of this vulture, has no current
					enRoute locations, has mines, and has been completed by the factory.
		*/
		bool isAvailable();

		//! Adds a position for this vulture to be en route to.
		/*!
			\param pos The position for this vulture to place a mine at.
		*/
		void addPosition(BWAPI::Position* pos);

		//! Removes a position that this vulture was en route to.
		/*!
			\param pos The position to clear from the enRoute list.
		*/
		bool removePosition(BWAPI::Position* pos);

		//! Checks the state of the vulture and issues a command accordingly each frame.
		/*!
			Should be called every frame.
			If the vulture is within range of placing a spider mine in one of its en route
			locations, it will stop moving and do so. 
			If the vulture has placed a mine, calling this function will do nothing for a constant
			number of frames, as issuing a new order will interrupt the mine placement.
			If the vulture has an en route location, is not moving, and is not placing a mine, then
			calling this function will make it move to the next location.
		*/
		void update();
	};

private:
	//! The set of vultures controlled by the player, each stored as a VultureMineCount.
	std::set<VultureMineCount*> vultures;

	//! The set of mines placed on the field.
	std::set<BWAPI::Unit*> mines;

	//! A map between each position where a mine needs to placed and its integer priority.
	std::map<BWAPI::Position*, int> desiredMineLocations;

	//! Tests if a vulture is en route to a position.
	/*!
		\param pos The position to test if a vulture is moving to place a mine.
		\return True if a vulture is on its way to the given position, false otherwise.
	*/
	bool vultureEnroute(BWAPI::Position* pos);
};
