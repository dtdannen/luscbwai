#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Squad.h"
#include <Arbitrator.h>
#include "ChokePriority.h"
#include <string>
//! This class manages the execution of the Slow Push.
/*!
	
*/


class SlowPushManager : Arbitrator::Controller<BWAPI::Unit*,double>
{
  public:
	enum Goals { SLOWPUSH, DEFEND };
	enum SquadBehavior { DEFENSIVE, AGGRESSIVE, STAY, FLEE };
	
    SlowPushManager(Arbitrator::Arbitrator<BWAPI::Unit*,double> *arbitrator, ChokePointAdvisor *chokePointAdvisor);
	void giveGoal(int g);
	virtual void update();
	virtual std::string getName() const;
	virtual std::string getShortName() const;
	virtual void onOffer(std::set<BWAPI::Unit*> units);
	virtual void onRevoke(BWAPI::Unit* unit, double bid);
	void onRemoveUnit(BWAPI::Unit* unit);

  private:
	int goal;
	Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
	ChokePointAdvisor* chokePointAdvisor;
	std::set<BWAPI::Unit*> armyUnits;
	std::vector<ChokePriority> squads;

	void distributeUnits(std::set<BWTA::Chokepoint*> chokepoints);
	std::set<BWTA::Chokepoint*> getNextChokepoints(); 
	bool readyForNewCommand();
	void advanceSlowPush(std::set<BWTA::Chokepoint*> nextChokepoints); 
	std::vector<ChokePriority> chokePrioritySet;
	//void update();
};

extern SlowPushManager* TheStrategyManager;
