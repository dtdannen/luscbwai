#include "LUSCBWAI.h"
#include "../Addons/Util.h"
using namespace BWAPI;



void BasicAIModule::onStart()
{

	this->showManagerAssignments=false;
	if (Broodwar->isReplay()) return;
	// Enable some cheat flags
	Broodwar->enableFlag(Flag::UserInput);
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Run BWTA
	BWTA::readMap();
	BWTA::analyze();
	this->analyzed=true;

	// Create managers
	this->buildManager       = new BuildManager(&this->arbitrator);
	this->techManager        = new TechManager(&this->arbitrator);
	this->upgradeManager     = new UpgradeManager(&this->arbitrator);
	this->scoutManager       = new ScoutManager(&this->arbitrator);
	this->workerManager      = new WorkerManager(&this->arbitrator);
	this->supplyManager      = new SupplyManager();
	this->baseManager        = new BaseManager();
	this->buildOrderManager  = new BuildOrderManager(this->buildManager,this->techManager,
									this->upgradeManager,this->workerManager,this->supplyManager);
	this->defenseManager     = new DefenseManager(&this->arbitrator);
	this->informationManager = InformationManager::create();
	this->borderManager      = BorderManager::create();
	this->unitGroupManager   = UnitGroupManager::create();
	this->enhancedUI         = new EnhancedUI();
	this->goalManager		   = new GoalManager(this->buildOrderManager);

	this->supplyManager->setBuildManager(this->buildManager);
	this->supplyManager->setBuildOrderManager(this->buildOrderManager);
	this->techManager->setBuildingPlacer(this->buildManager->getBuildingPlacer());
	this->upgradeManager->setBuildingPlacer(this->buildManager->getBuildingPlacer());
	this->workerManager->setBaseManager(this->baseManager);
	this->workerManager->setBuildOrderManager(this->buildOrderManager);
	this->baseManager->setBuildOrderManager(this->buildOrderManager);
	this->baseManager->setBorderManager(this->borderManager);
	this->defenseManager->setBorderManager(this->borderManager);

	// Zues controls the high level goal that guides the bot's actions
	Zeus::Instance().setMacroManager(goalManager);

	// get information about our bot
	BWAPI::Race race = Broodwar->self()->getRace();
	BWAPI::Race enemyRace = Broodwar->enemy()->getRace();
	BWAPI::UnitType workerType=race.getWorker();
	double minDist;
	BWTA::BaseLocation* natural=NULL;
	BWTA::BaseLocation* home=BWTA::getStartLocation(Broodwar->self());
	for(std::set<BWTA::BaseLocation*>::const_iterator 
			b=BWTA::getBaseLocations().begin();b!=BWTA::getBaseLocations().end();b++)
	{
		if (*b==home) continue;
		double dist=home->getGroundDistance(*b);
		if (dist>0)
		{
			if (natural==NULL || dist<minDist)
			{
				minDist=dist;
				natural=*b;
			}
		}
	}
	this->buildOrderManager->enableDependencyResolver();
	if (race == Races::Terran)
	{
		this->buildOrderManager->build(20,workerType,80);
		/*if (enemyRace == Races::Zerg)
		{
			this->buildOrderManager->buildAdditional(1,UnitTypes::Terran_Barracks,60);
			this->buildOrderManager->buildAdditional(9,UnitTypes::Terran_Marine,45);
			this->buildOrderManager->buildAdditional(1,UnitTypes::Terran_Refinery,42);
			this->buildOrderManager->buildAdditional(1,UnitTypes::Terran_Barracks,40);
			this->buildOrderManager->buildAdditional(1,UnitTypes::Terran_Academy,39);
			this->buildOrderManager->buildAdditional(9,UnitTypes::Terran_Medic,38);
			this->buildOrderManager->research(TechTypes::Stim_Packs,35);
			this->buildOrderManager->research(TechTypes::Tank_Siege_Mode,35);
			this->buildOrderManager->buildAdditional(3,UnitTypes::Terran_Siege_Tank_Tank_Mode,34);
			this->buildOrderManager->buildAdditional(2,UnitTypes::Terran_Science_Vessel,30);
			this->buildOrderManager->research(TechTypes::Irradiate,30);
			this->buildOrderManager->upgrade(1,UpgradeTypes::Terran_Infantry_Weapons,20);
			this->buildOrderManager->build(3,UnitTypes::Terran_Missile_Turret,13);
			this->buildOrderManager->upgrade(3,UpgradeTypes::Terran_Infantry_Weapons,12);
			this->buildOrderManager->upgrade(3,UpgradeTypes::Terran_Infantry_Armor,12);
			this->buildOrderManager->build(1,UnitTypes::Terran_Engineering_Bay,11);
			this->buildOrderManager->buildAdditional(40,UnitTypes::Terran_Marine,10);
			this->buildOrderManager->build(6,UnitTypes::Terran_Barracks,8);
			this->buildOrderManager->build(2,UnitTypes::Terran_Engineering_Bay,7);
			this->buildOrderManager->buildAdditional(10,UnitTypes::Terran_Siege_Tank_Tank_Mode,5);
		}
		else
		{

			this->buildOrderManager->buildAdditional(2,BWAPI::UnitTypes::Terran_Machine_Shop,70);
			this->buildOrderManager->buildAdditional(3,BWAPI::UnitTypes::Terran_Factory,60);
			this->buildOrderManager->research(TechTypes::Spider_Mines,55);
			this->buildOrderManager->research(TechTypes::Tank_Siege_Mode,55);
			this->buildOrderManager->buildAdditional(20,BWAPI::UnitTypes::Terran_Vulture,40);
			this->buildOrderManager->buildAdditional(20,
												BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode,40);
			this->buildOrderManager->upgrade(3,UpgradeTypes::Terran_Vehicle_Weapons,20);
		}*/
	}
	else // we are not Terran, alert user
	{
		std::string wrongRace = "Undefined behavior when bot does not play as Terran, bye!";
		Broodwar->sendText("%s",wrongRace.c_str());
	}

	this->workerManager->enableAutoBuild();
	this->workerManager->setAutoBuildPriority(80);
	this->baseManager->setRefineryBuildPriority(30);

	// BEGIN SLOW PUSH STRATEGY

	// 1. Start producing the right units:
	// Slow Push Goal to produce siege tanks, vultures, and marines
	std::map<BWAPI::UnitType, int> slowPushBuildGoal;
	slowPushBuildGoal[BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode] = 3;
	slowPushBuildGoal[BWAPI::UnitTypes::Terran_Vulture] = 1;
	slowPushBuildGoal[BWAPI::UnitTypes::Terran_Marine] = 4;
	this->goalManager->newGoal(slowPushBuildGoal);
}

BasicAIModule::~BasicAIModule()
{
	delete this->buildManager;
	delete this->techManager;
	delete this->upgradeManager;
	delete this->scoutManager;
	delete this->workerManager;
	delete this->supplyManager;
	delete this->buildOrderManager;
	delete this->baseManager;
	delete this->defenseManager;
	InformationManager::destroy();
	BorderManager::destroy();
	UnitGroupManager::destroy();
	delete this->enhancedUI;
}
void BasicAIModule::onEnd(bool isWinner)
{
	log("onEnd(%d)\n",isWinner);
}
void BasicAIModule::onFrame()
{
	if (Broodwar->isReplay()) return;
	if (!this->analyzed) return;
	this->buildManager->update();
	this->buildOrderManager->update();
	this->baseManager->update();
	this->workerManager->update();
	this->techManager->update();
	this->upgradeManager->update();
	this->supplyManager->update();
	//this->scoutManager->update();
	this->enhancedUI->update();
	this->borderManager->update();
	this->defenseManager->update();
	this->arbitrator.update();

	Zeus::Instance().update();

	if (Broodwar->getFrameCount()>24*50)
		scoutManager->setScoutCount(1);

	// Draw active/not active mode
	std::set<Unit*> units=Broodwar->self()->getUnits();
	if (this->showManagerAssignments)
	{
		for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++)
		{
			if (this->arbitrator.hasBid(*i))
			{
				int x=(*i)->getPosition().x();
				int y=(*i)->getPosition().y();
				std::list< std::pair< Arbitrator::Controller<BWAPI::Unit*,double>*, double> > 
														bids=this->arbitrator.getAllBidders(*i);
				int y_off=0;
				bool first = false;
				const char activeColor = '\x07', inactiveColor = '\x16';
				char color = activeColor;
				for(std::list<std::pair<Arbitrator::Controller<BWAPI::Unit*,
									double>*, double>>::iterator j=bids.begin();j!=bids.end();j++)
				{
					Broodwar->drawTextMap(x,y+y_off,"%c%s: %d",color,
											j->first->getShortName().c_str(),(int)j->second);
					y_off+=15;
					color = inactiveColor;
				}
			}
		}
	}

	// keep producing units
	/*std::map<BWAPI::UnitType, int> slowPushBuildGoal;
	slowPushBuildGoal[BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode] = 3;
	slowPushBuildGoal[BWAPI::UnitTypes::Terran_Vulture] = 1;
	slowPushBuildGoal[BWAPI::UnitTypes::Terran_Marine] = 4;
	this->goalManager->newGoal(slowPushBuildGoal);*/

	}

void BasicAIModule::onUnitDestroy(BWAPI::Unit* unit)
{
	if (Broodwar->isReplay()) return;
	this->arbitrator.onRemoveObject(unit);
	this->buildManager->onRemoveUnit(unit);
	this->techManager->onRemoveUnit(unit);
	this->upgradeManager->onRemoveUnit(unit);
	this->workerManager->onRemoveUnit(unit);
	this->scoutManager->onRemoveUnit(unit);
	this->defenseManager->onRemoveUnit(unit);
	this->informationManager->onUnitDestroy(unit);
	this->baseManager->onRemoveUnit(unit);
}

void BasicAIModule::onUnitDiscover(BWAPI::Unit* unit)
{
	if (Broodwar->isReplay()) return;
	this->informationManager->onUnitDiscover(unit);
	this->unitGroupManager->onUnitDiscover(unit);
}
void BasicAIModule::onUnitEvade(BWAPI::Unit* unit)
{
	if (Broodwar->isReplay()) return;
	this->informationManager->onUnitEvade(unit);
	this->unitGroupManager->onUnitEvade(unit);
}

void BasicAIModule::onUnitMorph(BWAPI::Unit* unit)
{
	if (Broodwar->isReplay()) return;
	this->unitGroupManager->onUnitMorph(unit);
}
void BasicAIModule::onUnitRenegade(BWAPI::Unit* unit)
{
	if (Broodwar->isReplay()) return;
	this->unitGroupManager->onUnitRenegade(unit);
}

void BasicAIModule::onSendText(std::string text)
{
	if (Broodwar->isReplay())
	{
		Broodwar->sendText("%s",text.c_str());
		return;
	}
	UnitType type=UnitTypes::getUnitType(text);
	if (text=="debug")
	{
		if (this->showManagerAssignments==false)
		{
			this->showManagerAssignments=true;
			this->buildOrderManager->setDebugMode(true);
			this->scoutManager->setDebugMode(true);
		}
		else
		{
			this->showManagerAssignments=false;
			this->buildOrderManager->setDebugMode(false);
			this->scoutManager->setDebugMode(false);
		}
		Broodwar->printf("%s",text.c_str());
		return;
	}
	if (text=="expand")
	{
		this->baseManager->expand();
		Broodwar->printf("%s",text.c_str());
		return;
	}
	if (type!=UnitTypes::Unknown)
	{
		this->buildOrderManager->buildAdditional(1,type,300);
	}
	else
	{
		TechType type=TechTypes::getTechType(text);
		if (type!=TechTypes::Unknown)
		{
			this->techManager->research(type);
		}
		else
		{
			UpgradeType type=UpgradeTypes::getUpgradeType(text);
			if (type!=UpgradeTypes::Unknown)
			{
				this->upgradeManager->upgrade(type);
			}
			else
				Broodwar->printf("You typed '%s'!",text.c_str());
		}
	}
	Broodwar->sendText("%s",text.c_str());
}
