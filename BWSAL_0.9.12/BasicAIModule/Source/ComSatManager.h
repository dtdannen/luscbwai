#pragma once
#include "Common.h"
#include "Arbitrator.h"

class ComSatManager : public Arbitrator::Controller<BWAPI::Unit*,double>
{
private:
	std::set<BWAPI::Unit *> bases;
	std::set<BWAPI::Unit *> comSats;
	std::set<BWAPI::Unit *> stealthDetectors;
	std::set<BWAPI::Unit *> noEnergyComs;

public:
	//void update();
	void onUnitShow(BWAPI::Unit * unit);
	bool requestDetectionSweep(BWAPI::Position position);
	bool requestSweep(BWAPI::Position position);
	static ComSatManager & Instance();
	virtual void update();
	virtual void onOffer(std::set<BWAPI::Unit*> units);
    virtual void onRevoke(BWAPI::Unit* unit, double bid);
	virtual std::string getName() const;

	Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;

};

