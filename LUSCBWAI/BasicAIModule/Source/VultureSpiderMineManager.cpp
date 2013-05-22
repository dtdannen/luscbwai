#include "VultureSpiderMineManager.h"

VultureSpiderMineManager::VultureSpiderMineManager(
	Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator)
{
	this->arbitrator = arbitrator;
}

void VultureSpiderMineManager::destroy(BWAPI::Unit* u)
{
	if(u->getType() == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine)
	{
		//If a spider mine is destroyed, it is desired again with 100 priority.
		this->addDesiredMineLocation(&(u->getPosition()), 100);
		this->mines.erase(u);
	}
	else if(u->getType() == BWAPI::UnitTypes::Terran_Vulture)
	{
		//Remove the appropriate VultureMineCount.
		VultureMineCount* v = NULL;
		for(std::set<VultureMineCount*>::iterator i = vultures.begin(); i != vultures.end(); i++)
			if((*i)->vulture == u)
				v = (*i);
		if(v != NULL)
			this->vultures.erase(v);
	}
}

void VultureSpiderMineManager::create(BWAPI::Unit* u)
{
	if(u->getType() == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine)
	{
		//Count down a mine from the vulture that placed it:
		for(std::set<VultureMineCount*>::iterator i = vultures.begin(); i != vultures.end(); i++)
			if((*i)->removePosition(&u->getPosition()))
				(*i)->numMines--;

		//Remove the mine location from desiredMineLocations:
		for(std::map<BWAPI::Position*, int>::iterator i = desiredMineLocations.begin(); 
													  i != desiredMineLocations.end(); i++)
		{
			if((*i).first->x() == u->getPosition().x() && (*i).first->y() == u->getPosition().y())
			{
				this->desiredMineLocations.erase(i);
				break;
			}
		}

		//Keep track of the mine:
		this->mines.insert(u);
	}
	else if(u->getType() == BWAPI::UnitTypes::Terran_Vulture)
	{
		this->vultures.insert(new VultureMineCount(u));
	}
}

void VultureSpiderMineManager::addDesiredMineLocation(BWAPI::Position* position, int priority)
{
	//Make sure the position is walkable.
	if(!BWAPI::Broodwar->isWalkable(position->x()/8, position->y()/8))
		return;
	//Make sure the position has not already been added.
	for(std::map<BWAPI::Position*, int>::iterator t = desiredMineLocations.begin(); 
												  t != desiredMineLocations.end(); t++)
		if(position == (*t).first)
			return;

	this->desiredMineLocations.insert(std::pair<BWAPI::Position*, int>(position, priority));
}

void VultureSpiderMineManager::addDesiredMineLocations(std::set<BWAPI::Position*> positions, 
													   int priority)
{
	for(std::set<BWAPI::Position*>::iterator i = positions.begin(); i != positions.end(); i++)
		addDesiredMineLocation(*i, priority);
}

void VultureSpiderMineManager::onOffer(std::set<BWAPI::Unit*> units)
{
	for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
		for(std::set<VultureMineCount*>::iterator i = vultures.begin(); i != vultures.end(); i++)
			if(!(*i)->controls && (*i)->vulture == (*u))
				(*i)->controls = true;
}

void VultureSpiderMineManager::onRevoke(BWAPI::Unit* unit, double bid)
{
	for(std::set<VultureMineCount*>::iterator i = vultures.begin(); i != vultures.end(); i++)
		if((*i)->vulture == unit)
			(*i)->controls = false;
}

void VultureSpiderMineManager::update()
{
	for(std::set<VultureMineCount*>::iterator i = vultures.begin(); i != vultures.end(); i++)
		(*i)->update();

	//Drop control of any vultures without mines:
	for(std::set<VultureMineCount*>::iterator i = vultures.begin(); i != vultures.end(); i++)
	{
		if((*i)->controls && (*i)->numMines == 0)
		{
			this->arbitrator->removeBid(this, (*i)->vulture);
			(*i)->controls = false;
		}
	}
	//Get all vultures that we control, are not enroute, and have mines, 
	//as well as all vultures with mines that we do not control:
	std::set<BWAPI::Unit*> uncontrolledVulturesWithMines;                                          
	std::set<VultureMineCount*> availableVultures;
	for(std::set<VultureMineCount*>::iterator i = vultures.begin(); i != vultures.end(); i++)
	{
		if((*i)->isAvailable())
			availableVultures.insert(*i);
		if(!(*i)->controls && (*i)->numMines > 0)
			uncontrolledVulturesWithMines.insert((*i)->vulture);
	}

	//If we have no available vultures, request more:
	if(availableVultures.size() == 0)
	{
		this->arbitrator->setBid(this, uncontrolledVulturesWithMines, 100);
	}
	//If we have available vultures and unclaimed locations to place mines:
	else if(this->desiredMineLocations.size() > 0)
	{
		//Get highest priority position that is unclaimed:
		BWAPI::Position* pos;
		int maxPriority = 0;
		for(std::map<BWAPI::Position*, int>::iterator i = desiredMineLocations.begin(); 
			i != desiredMineLocations.end(); i++)
		{
			if(!vultureEnroute((*i).first) && (*i).second > maxPriority)
			{
				pos = (*i).first;
				maxPriority = (*i).second;
			}
		}

		//Get Vulture closest to the highest priority position:
		VultureMineCount* closestVulture;
		int closestDistance = -1;
		for(std::set<VultureMineCount*>::iterator i = availableVultures.begin(); 
												  i != availableVultures.end(); i++)
		{
			int distance = (*i)->vulture->getDistance(*pos);
			if(closestDistance == -1 || distance < closestDistance)
			{
				closestDistance = distance;
				closestVulture = (*i);
			}
		}
		//Send the Vulture to that location:
		closestVulture->addPosition(pos);

		//For each additional mine that the vulture has, add the mine positions closest to the one
		//that has already been added.
		for(int count = closestVulture->numMines-1; count > 0; count--)
		{
			bool selected = false;
			BWAPI::Position* pos1 = NULL;
			int distance = 0;
			for(std::map<BWAPI::Position*, int>::iterator i = desiredMineLocations.begin();
														  i != desiredMineLocations.end(); i++)
			{
				BWAPI::Position* location = (*i).first;
				if(location != pos && !vultureEnroute(location) && 
					(pos1 == NULL || pos->getDistance(*location) < pos->getDistance(*pos1)))
				{
						pos1 = location;
						selected = true;
				}
			}
			//It is possible that there are no more mine locations available, so this is checked.
			if(selected)
			{
				closestVulture->addPosition(pos1);
			}
		}
	}
}

std::string VultureSpiderMineManager::getName() const
{
	return "Vulture Spider Mine Manager";
}
std::string VultureSpiderMineManager::getShortName() const
{
	return "Mines";
}

bool VultureSpiderMineManager::vultureEnroute(BWAPI::Position* pos)
{
	for(std::set<VultureMineCount*>::iterator i = vultures.begin(); i != vultures.end(); i++)
		for(std::list<BWAPI::Position*>::iterator mineIterator = (*i)->enRoute.begin(); 
											mineIterator != (*i)->enRoute.end(); mineIterator++)
			if((*mineIterator) == pos)
				return true;
	return false;
}

bool VultureSpiderMineManager::VultureMineCount::isAvailable() 
{
	return vulture->isCompleted() && controls && numMines > 0 && enRoute.empty(); 
}

void VultureSpiderMineManager::VultureMineCount::addPosition(BWAPI::Position* pos) 
{
	enRoute.push_back(pos);
}

bool VultureSpiderMineManager::VultureMineCount::removePosition(BWAPI::Position* pos) 
{
	for(std::list<BWAPI::Position*>::iterator i = enRoute.begin(); i != enRoute.end(); i++)
	{
		if((*i)->getDistance(*pos) == 0)
		{
			enRoute.erase(i);
			return true;
		}
	}
	return false;
}

void VultureSpiderMineManager::VultureMineCount::update()
{
	if(!enRoute.empty() && numMines == 0)
		enRoute.clear();
	if(!vulture->isCompleted() || enRoute.empty() || !controls || numMines == 0)
		return;
	if(placingMine > 0)
	{
		placingMine--;
		return;
	}
	if(!isMoving)
	{
		vulture->move(*enRoute.front(), true);
		isMoving = true;
	}
	for(std::list<BWAPI::Position*>::iterator i = enRoute.begin(); i != enRoute.end(); i++)
	{
		//10 is just an arbitrary tiny number, such that we can be sure that the vulture is
		//within range of the location to place the spider mine immediately.
		const int SPIDER_MINE_RANGE = 10;
		if(vulture->getPosition().getDistance(**i) < SPIDER_MINE_RANGE)
		{
			bool canUse = vulture->useTech(BWAPI::TechTypes::Spider_Mines, **i);
			if(canUse)
			{
				//120 seems to be a safe number of frames to delay after placing a mine to ensure
				//that a new order will not be given to interrupt the mine placement.
				const int SPIDER_MINE_FRAME_COOLDOWN = 120;
				placingMine = SPIDER_MINE_FRAME_COOLDOWN;
				isMoving = false;
				return;
			}
		}
	}
}