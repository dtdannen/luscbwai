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
	for each (BWTA::Chokepoint const * c in BWTA::getChokepoints())
	{
		// const std::pair<const BWAPI::Region *, const BWAPI::Region *> p = c->getRegions();

		// Map first to second, then vice versa
		// This looks really gross, but when I tried to make it work like above, it just kept yelling, so I quit.
		nodeMap[locationMap[c->getRegions().first->getCenter()]]->addNeighbor(locationMap[c->getRegions().second->getCenter()]);
		nodeMap[locationMap[c->getRegions().second->getCenter()]]->addNeighbor(locationMap[c->getRegions().first->getCenter()]);
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
			BWAPI::Broodwar->drawCircleMap(it->second->getCenter().x(), it->second->getCenter().y(), 10, BWAPI::Colors::Green, true);
		}
		else if (it->second->getColor() == ORANGE)
		{
			BWAPI::Broodwar->drawCircleMap(it->second->getCenter().x(), it->second->getCenter().y(), 10, BWAPI::Colors::Orange, true);
		}
		else if (it->second->getColor() == RED)
		{
			BWAPI::Broodwar->drawCircleMap(it->second->getCenter().x(), it->second->getCenter().y(), 10, BWAPI::Colors::Red, true);
		}
		else
		{
			BWAPI::Broodwar->drawCircleMap(it->second->getCenter().x(), it->second->getCenter().y(), 10, BWAPI::Colors::White, true); //otherwise the fog of war makes them invis
		}

		for each (int i in it->second->getNeighbors())
		{
			BWAPI::Position pos = getNodeCenter(i);
			BWAPI::Position pos2 = it->second->getCenter();
			BWAPI::Broodwar->drawLineMap(pos.x(), pos.y(), pos2.x(), pos2.y(), BWAPI::Colors::White);
		}
	}
}


void ColorGraph::processColorExpiration()
{
	// leave red and green alone
	// kill orange after 300 frames
	for (std::map<int, ColorNode *>::iterator it = nodeMap.begin(); it != nodeMap.end(); ++it)
	{
		if (it->second->getColor() == ORANGE && BWAPI::Broodwar->getFrameCount() - it->second->getLastFrameUpdated() > 300)
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
