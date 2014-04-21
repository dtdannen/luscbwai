#include <list>
#include <BWTA.h>
#include "ColorGraph.h"
#include <algorithm>

#pragma once

class FrontierAdvisor
{
private:
	FrontierAdvisor(void);
	~FrontierAdvisor(void);
	double calculateNodeValue(int id);

	std::set<int> frontier; // holds the nodes that we consider to be part of our frontier
	std::set<int> controlled; // holds the nodes behind the frontier

public:
	static FrontierAdvisor &	Instance();
	int getNextNodeId();
	void addNodeToFrontier(int id);
	void recalculateFrontier();
	std::set<int> getFrontier();
	bool frozen;
};

