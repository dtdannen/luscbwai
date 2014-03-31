#include "FrontierAdvisor.h"


FrontierAdvisor::FrontierAdvisor(void)
{
	//frontier.push_back(ColorGraph::Instance().getNodeAtPosition(BWTA::getStartLocation(BWAPI::Broodwar->self())->getPosition()));
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
	double maxScore = 0;
	
	//// get the min distance
	//for each (int node in frontier)
	//{
	//	for each (int neighbor in ColorGraph::Instance().getNodeNeighbors(node))
	//	{
	//		if (std::find(controlled.begin(), controlled.end(), neighbor) == controlled.end() && std::find(frontier.begin(), frontier.end(), neighbor) == frontier.end())
	//		{
	//			double score = calculateNodeValue(neighbor);
	//			if (score < minScore)
	//			{
	//				minScore = score;
	//				id = neighbor;
	//			}
	//		}
	//	}
	//}

	for each (int node in frontier)
	{
		double importance = ColorGraph::Instance().getNodeImportance(id);
		if (importance > maxScore)
		{
			id = node;
			maxScore = importance;
			//BWAPI::Broodwar->printf("Temp goal Node: %d %f", node, importance);
			
		}
	}

	return id;
}

std::set<int> FrontierAdvisor::getFrontier()
{
	return std::set<int>(frontier);
}

void FrontierAdvisor::addNodeToFrontier(int id)
{
	if (std::find(frontier.begin(), frontier.end(), id) == controlled.end())
	{
		frontier.insert(id);
	}
}


void FrontierAdvisor::recalculateFrontier()
{
	//for each (int id in frontier)
	//{
	//	bool contained = true;
	//	for each (int neighbor in ColorGraph::Instance().getNodeNeighbors(id))
	//	{
	//		if (std::find(controlled.begin(), controlled.end(), neighbor) == controlled.end() && std::find(frontier.begin(), frontier.end(), neighbor) == frontier.end())
	//		{
	//			contained = false;
	//		}
	//	}

	//	if (contained)
	//	{
	//		frontier.erase(std::find(frontier.begin(), frontier.end(), id));
	//		controlled.push_back(id);
	//	}
	//}

	//BWAPI::Broodwar->printf("Recalculating Frontier");
	controlled.clear();
	frontier.clear();

	for (int i = 0; i < ColorGraph::Instance().size(); i++)
	{
		if (ColorGraph::Instance().getNodeColor(i) == GREEN)
		{
			controlled.insert(i);
		}
	}

	for each (int id in controlled)
	{
		for each (int neighbor in ColorGraph::Instance().getNodeNeighbors(id))
		{
			if (ColorGraph::Instance().getNodeColor(neighbor) != GREEN)	
			{
				frontier.insert(neighbor);
			}
		}
	}
}

FrontierAdvisor & FrontierAdvisor::Instance() 
{
	static FrontierAdvisor instance;
	return instance;
}