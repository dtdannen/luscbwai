#pragma once

#include "BWTA.h"
#include "ColorNode.h"

class ColorGraph
{
	std::map<int, ColorNode *> nodeMap;
	std::map<BWAPI::Position, int> locationMap;
	int graphSize;

	ColorGraph(void);
	~ColorGraph(void);

public:
	static ColorGraph &	Instance();

	NodeColor getNodeColor(int id);
	BWAPI::Position getNodeCenter(int id);
	int getLastUpdatedFrame(int id);
	std::list<int> getNodeNeighbors(int id);
	int size();
	int getNodeAtPosition(BWAPI::Position position);

	void updateColors();
};

