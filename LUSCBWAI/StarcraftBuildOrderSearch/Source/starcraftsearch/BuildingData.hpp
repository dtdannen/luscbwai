#ifndef BUILDING_DATA_H
#define BUILDING_DATA_H

#include <string.h>
#include <queue>
#include <algorithm>

#include "BWAPI.h"
#include "ActionSet.hpp"
#include "StarcraftAction.hpp"
#include "StarcraftData.hpp"
#include "StarcraftState.hpp"

namespace BuildOrderSearch
{
class BuildingStatus
{

public:

	// the type of unit this is
	Action type;
	
	// the number of frames remaining (from currentFrame) until this building is free
	FrameCountType timeRemaining;

	// whether or not this building has an add on attached to it
	bool hasAddOn;

	BuildingStatus() : type(0), timeRemaining(0) 
	{
		memset(this, 0, sizeof(*this));
	}
	
	~BuildingStatus() {}
	BuildingStatus(Action t) : type(t), timeRemaining(0), hasAddOn(false) {}
	BuildingStatus(Action t, FrameCountType time) : type(t), timeRemaining(time), hasAddOn(false) {}
	BuildingStatus(Action t, FrameCountType time, bool addOn) : type(t), timeRemaining(time), hasAddOn(addOn) {}
};

class BuildingData
{

	BuildingStatus buildings[MAX_BUILDINGS];

public:

	int numBuildings;
	std::vector<std::pair<int, int>> availableForAddons;

	BuildingData() : numBuildings(0) {}

	void addBuilding(const Action t)
	{
		assert(DATA[t].isBuilding());
		assert(numBuildings < (MAX_BUILDINGS - 1));
	
		buildings[numBuildings++] = BuildingStatus(t, 0);
	}

	void addBuilding(const Action t, const FrameCountType timeUntilFree)
	{
		assert(DATA[t].isBuilding());
		assert(numBuildings < (MAX_BUILDINGS - 1));

		buildings[numBuildings++] = BuildingStatus(t, timeUntilFree);
	}

	void addBuilding(const Action t, const FrameCountType timeUntilFree, bool hasAddon)
	{
		assert(DATA[t].isBuilding());
		assert(numBuildings < (MAX_BUILDINGS -1));

		buildings[numBuildings++] = BuildingStatus(t, timeUntilFree, hasAddon);
	}

	bool canAddOn(const Action t) const
	{
		assert(DATA[t].isAddOn());

		BWAPI::UnitType whatBuilds = DATA[t].whatBuilds();
		Action buildingAddedOn = 255;

		// find the action number of the building to add on to
		for(Action a(0); a < DATA.size(); ++a)
		{
			if(DATA[a].getUnitType() == whatBuilds)
			{
				buildingAddedOn = a;
				break;
			}
		}

		assert(buildingAddedOn < 255);

		// iterate through the buildings and try to find one of the correct type that is completed and doesn't have an add on
		for (int i=0; i<numBuildings; ++i)
		{
			if (buildings[i].type == buildingAddedOn && buildings[i].timeRemaining == 0 && !buildings[i].hasAddOn)
			{
				return true;
			}
		}

		// if a building to add on wasn't found, we can't add on
		return false;
	}

	void simulateAddOn(const Action t)
	{
		assert(DATA[t].isAddOn());

		BWAPI::UnitType whatBuilds = DATA[t].whatBuilds();
		Action buildingAddedOn = 255;

		// find the action number of the building to add on to
		for(Action a(0); a < DATA.size(); ++a)
		{
			if(DATA[a].getUnitType() == whatBuilds)
			{
				buildingAddedOn = a;
				break;
			}
		}

		assert(buildingAddedOn < 255);

		// find the building to add on to. this should not fail since the action would be illegal if it could not get here
		for (int i=0; i<numBuildings; ++i)
		{
			if (buildings[i].type == buildingAddedOn && buildings[i].timeRemaining == 0 && !buildings[i].hasAddOn)
			{
				buildings[i].hasAddOn = true;
				return;
			}
		}

		// we should not get here
		assert(false);
	}

	const BuildingStatus & getBuilding(int i) const
	{
		assert(i >= 0 && i < MAX_BUILDINGS);
		
		return buildings[i];
	}
	
	// get the index of the first free building of type t
	int getFreeBuilding(const Action t) const
	{
		// for each building
		for (int i=0; i<numBuildings; ++i)
		{
			if (buildings[i].type == t && buildings[i].timeRemaining == 0)
			{
				return i;
			}
		}
		
		return BUILDING_ERROR;
	}
	
	// gets the time until building of type t is free
	// this will only ever be called if t exists, so min will always be set to a lower value
	FrameCountType timeUntilFree(const Action t) const
	{
		FrameCountType min = 99999;
	
		for (int i=0; i<numBuildings; ++i)
		{
			if (buildings[i].type == t)
			{
				min = (buildings[i].timeRemaining < min) ? buildings[i].timeRemaining : min;
			}
		}
		
		assert(min != 9999999);
	
		return min;
	}

	// queue an action
	void queueAction(const Action a)
	{	
		// for each building
		for (int i=0; i<numBuildings; ++i)
		{
		
			// if the building is free and of the correct type
			if (buildings[i].type == DATA[a].whatBuildsAction() && buildings[i].timeRemaining == 0)
			{
				// queue it here
				buildings[i].timeRemaining = (unsigned short)DATA[a].buildTime();
				// if we are adding and add on to this building, indicate that it has an add on
				if(DATA[a].isAddOn())
					buildings[i].hasAddOn = true;
				//printf("\t\t%s is ready! Using it for %s.\n", DATA[buildings[i].type].getName().c_str(), DATA[a].getName().c_str());
				return;
			}
		}
		
		printf("Assert Incoming: %s which needed %s but didn't find it.\n", DATA[a].getName().c_str(), DATA[DATA[a].whatBuildsAction()].getName().c_str());
		printBuildingInformation();
		
		// this method should always work since we have fast forwarded to the correct point in time
		assert(false);
	}
	
	void fastForwardBuildings(const FrameCountType frames)
	{
		for (int i=0; i<numBuildings; ++i)
		{
			buildings[i].timeRemaining = ((buildings[i].timeRemaining - frames) > 0) ? (buildings[i].timeRemaining - frames) : 0;
			if (GSN_DEBUG && frames > 0) printf("\t\t\tBuilding %s reduced to %d frames remaining\n", DATA[buildings[i].type].getName().c_str(), buildings[i].timeRemaining);
		}
	}
	
	void printBuildingInformation() const
	{
		for (int i=0; i<numBuildings; ++i)
		{
			if (buildings[i].timeRemaining == 0) 
			{
				printf("BUILDING INFO: %s is free to assign\n", DATA[buildings[i].type].getName().c_str());
			}
			else 
			{
				printf("BUILDING INFO: %s will be free in %d frames\n", DATA[buildings[i].type].getName().c_str(), buildings[i].timeRemaining);
			}
		}
		
		printf("-----------------------------------------------------------\n\n");
	}
};
}
#endif
