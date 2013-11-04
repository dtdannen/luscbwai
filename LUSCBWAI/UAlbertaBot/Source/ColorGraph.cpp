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

void ColorGraph::updateColors()
{
	// TODO implement
	// Needs to get information about what's in each region and how confident we are
	// that we control it. Most likely this should be stored in the StrategyManager
	// or InformationManager
}

int ColorGraph::getNodeAtPosition(BWAPI::Position position)
{
	return locationMap[position];
}

ColorGraph & ColorGraph::Instance() 
{
	static ColorGraph instance;
	return instance;
}

ColorNode * ColorGraph::getNodeById(int id) {
	return nodeMap[id];
}