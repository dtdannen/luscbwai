#pragma once
#include <BWAPI.h>
#include <BWTA.h>

class ChokePointAdvisor
{
private:
	/**
	 * Set of enemy bases.
	 */
	std::set<BWTA::BaseLocation*> enemyBases;
public:
	/**
	 * Constructor--does nothing right now.
	 */
	ChokePointAdvisor(void);
	/**
	 * Destructor--does nothing right now.
	 */
	~ChokePointAdvisor(void);

	/**
	 * Update--does nothing right now.
	 */
	void update();
	/**
	 * Adds a base location to enemyBases.
	 */
	void addBase(BWTA::BaseLocation* base);
	/**
	 * Removes a base location to enemyBases.
	 */
	void removeBase(BWTA::BaseLocation* base);
	/**
	 * Main poll method - wrapper method for basic polling.
	 */
	int pollChoke(BWTA::Chokepoint* choke);
	/**
	 * Gives set of contested chokes.
	 */
	std::set<BWTA::Chokepoint*> contestedChokes();
	/**
	 * Gives set of held chokes.
	 */
	std::set<BWTA::Chokepoint*> heldChokes();
	/**
	 * Gives set of controlled chokes.
	 */
	std::set<BWTA::Chokepoint*> controlledChokes();
	/**
	 * Gives maximum priority choke.
	 */
	BWTA::Chokepoint* priorityChoke();
	/**
	 * Gives a buffer containing info about a choke point.
	 * States whether a choke is contested, held, or controlled.
	 * These may not always be mutually exclusive, hence the
	 * use of a buffer of bits.
	 */
	int chokeData(BWTA::Chokepoint* choke);
	/**
	 * Gives the minimum distances to enemy bases.
	 */
	double distToEnemy(BWTA::Chokepoint* choke);

private:
	/**
	 * Localized polling system.  Fast; does not use info about other regions.
	 */
	int quickPoll(BWTA::Chokepoint* choke);
};

