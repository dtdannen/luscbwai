#pragma once

#include "BWTA.h"
#include "ColorNode.h"

class ColorGraph
{
	std::map<int, ColorNode *> nodeMap;
	std::map<BWAPI::Position, int> locationMap;

public:
	ColorGraph(void);
	~ColorGraph(void);

	NodeColor getNodeColor(int id);
	BWAPI::Position getNodeCenter(int id);
	int getLastUpdatedFrame(int id);
	std::list<int> getNodeNeighbors(int id);

	void updateColors();
};

