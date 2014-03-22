#pragma once
#include "ColorGraph.h"
#include <vector>

class ImportanceScoreAdvisor
{

private:
	std::vector<int> baseNodes;

	ImportanceScoreAdvisor(void);
	~ImportanceScoreAdvisor(void);

	double calculateNodeValue(int id);

public:
	static ImportanceScoreAdvisor & Instance();

	void update();
};

