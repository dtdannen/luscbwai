#include "ColorGraph.h"


ColorGraph::ColorGraph(void)
{
	int nodeId = 0;

	// Create all nodes
	for each (BWTA::Region * region in BWTA::getRegions())
	{
		ColorNode * node = new ColorNode(nodeId);
		node->setColor(BLACK);
		node->setCenter(region->getCenter());

		locationMap[node->getCenter()] = node->getId();
		nodeMap[node->getId()] = node;

		nodeId++;
	}

	graphSize = nodeId;

	// Map nodes to connected nodes
	for each (BWTA::Region * region in BWTA::getRegions())
	{
		int node = locationMap[region->getCenter()];
		
		for each (BWTA::Region * r in region->getReachableRegions())
		{
			nodeMap[node]->addNeighbor(locationMap[r->getCenter()]);
		}
	}
}

ColorGraph::~ColorGraph(void)
{
}

NodeColor ColorGraph::getNodeColor(int id)
{
	return nodeMap[id]->getColor();
}

void ColorGraph::setNodeColor(int id, NodeColor color)
{
	nodeMap[id]->setColor(color);
}

BWAPI::Position ColorGraph::getNodeCenter(int id)
{
	return nodeMap[id]->getCenter();
}

int ColorGraph::getLastUpdatedFrame(int id)
{
	return nodeMap[id]->getLastFrameUpdated();
}

std::list<int> ColorGraph::getNodeNeighbors(int id)
{
	return nodeMap[id]->getNeighbors();
}

int ColorGraph::size()
{
	return graphSize;
}

int ColorGraph::getNodeAtPosition(BWAPI::Position position)
{
	BWAPI::Position p = BWTA::getRegion(position)->getCenter();
	return locationMap[p];
}

void ColorGraph::drawGraphColors()
{
	for (std::map<int, ColorNode *>::iterator it = nodeMap.begin(); it != nodeMap.end(); ++it)
	{
		if (it->second->getColor() == GREEN)
		{
			BWAPI::Broodwar->drawCircleMap(it->second->getCenter().x(), it->second->getCenter().y(), 50, BWAPI::Colors::Green, true);
		}
		else if (it->second->getColor() == ORANGE)
		{
			BWAPI::Broodwar->drawCircleMap(it->second->getCenter().x(), it->second->getCenter().y(), 50, BWAPI::Colors::Orange, true);
		}
		else if (it->second->getColor() == RED)
		{
			BWAPI::Broodwar->drawCircleMap(it->second->getCenter().x(), it->second->getCenter().y(), 50, BWAPI::Colors::Red, true);
		}
		else
		{
			BWAPI::Broodwar->drawCircleMap(it->second->getCenter().x(), it->second->getCenter().y(), 50, BWAPI::Colors::White, true); //otherwise the fog of war makes them invis
		}
	}
}


void ColorGraph::processColorExpiration()
{
	// leave red and green alone
	// kill orange after 300 frames
	for (std::map<int, ColorNode *>::iterator it = nodeMap.begin(); it != nodeMap.end(); ++it)
	{
		if (it->second->getColor() == ORANGE && BWAPI::Broodwar->getFrameCount() - it->second->getLastFrameUpdated() > 150)
		{
			it->second->setColor(BLACK);
		}
	}
}


ColorGraph & ColorGraph::Instance() 
{
	static ColorGraph instance;
	return instance;
}
