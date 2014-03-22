#include "ImportanceScoreAdvisor.h"

void ImportanceScoreAdvisor::update()
{
	baseNodes.clear();
	for (int i = 0; i < ColorGraph::Instance().size(); i++)
	{
		if (ColorGraph::Instance().getNodeColor(i) == RED)
		{
			baseNodes.push_back(i);
		}
	}
	
	for each (BWAPI::Unit * unit in BWAPI::Broodwar->self()->getUnits())
	{
		if (unit->getType() == BWAPI::UnitTypes::Terran_Command_Center)
		{
			baseNodes.push_back(ColorGraph::Instance().getNodeAtPosition(unit->getPosition()));
		}
	}

	if (!baseNodes.empty())
	{
		for (int i = 0; i < ColorGraph::Instance().size(); i++)
		{
			double imp = calculateNodeValue(i);
			ColorGraph::Instance().setNodeImportance(i, imp);
			//BWAPI::Broodwar->printf("Setting %d to %f", i, imp);
		}
	}
}

double ImportanceScoreAdvisor::calculateNodeValue(int id)
{
	double score = 0;
	BWAPI::Position p = ColorGraph::Instance().getNodeCenter(id);

	for each (int id in baseNodes)
	{
		score += p.getApproxDistance(ColorGraph::Instance().getNodeCenter(id));
	}
	return 1 / score;
}

ImportanceScoreAdvisor::ImportanceScoreAdvisor(void)
{
}


ImportanceScoreAdvisor::~ImportanceScoreAdvisor(void)
{
}

ImportanceScoreAdvisor & ImportanceScoreAdvisor::Instance() 
{
	static ImportanceScoreAdvisor instance;
	return instance;
}
