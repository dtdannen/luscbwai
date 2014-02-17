#include "FrontierAdvisor.h"


FrontierAdvisor::FrontierAdvisor(void)
{
	frontier.push_back(ColorGraph::Instance().getNodeAtPosition(BWTA::getStartLocation(BWAPI::Broodwar->self())->getPosition()));
}


FrontierAdvisor::~FrontierAdvisor(void)
{
}

double FrontierAdvisor::calculateNodeValue(int id)
{
	BWTA::BaseLocation * base = BWTA::getStartLocation(BWAPI::Broodwar->self());
	BWAPI::Position p = ColorGraph::Instance().getNodeCenter(id);
	return p.getApproxDistance(base->getPosition());
}

int FrontierAdvisor::getNextNodeId()
{
	int id = 0;
	double minScore = std::numeric_limits<double>::max();
	
	// get the min distance
	for each (int node in frontier)
	{
		for each (int neighbor in ColorGraph::Instance().getNodeNeighbors(node))
		{
			if (std::find(controlled.begin(), controlled.end(), neighbor) == controlled.end() && std::find(frontier.begin(), frontier.end(), neighbor) == frontier.end())
			{
				double score = calculateNodeValue(neighbor);
				if (score < minScore)
				{
					minScore = score;
					id = neighbor;
				}
			}
		}
	}

	return id;
}

void FrontierAdvisor::addNodeToFrontier(int id)
{
	if (std::find(frontier.begin(), frontier.end(), id) == controlled.end())
	{
		frontier.push_back(id);
	}
}


void FrontierAdvisor::recalculateFrontier()
{
	for each (int id in frontier)
	{
		bool contained = true;
		for each (int neighbor in ColorGraph::Instance().getNodeNeighbors(id))
		{
			if (std::find(controlled.begin(), controlled.end(), neighbor) == controlled.end() && std::find(frontier.begin(), frontier.end(), neighbor) == frontier.end())
			{
				contained = false;
			}
		}

		if (contained)
		{
			frontier.erase(std::find(frontier.begin(), frontier.end(), id));
			controlled.push_back(id);
		}
	}
}

FrontierAdvisor & FrontierAdvisor::Instance() 
{
	static FrontierAdvisor instance;
	return instance;
}