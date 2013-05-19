#pragma once
#include "Common.h"


/*
	Good extensions:
	Regional safety
	Enemy squad tracking
	Enemy squad trajectory prediction (since most AIs will undoubtedly use shortest pathing)
*/

class SafetyAdvisor
{
public:
	static SafetyAdvisor & Instance();
	double safetyMeasure();
	void countUnits();

	void OnUpdate();
	void OnUnitDestroyed(BWAPI::Unit * u);


private:

	int visibleSupply;
	int ourSupply;

	double unitWeight;
	double visibleWeight;
	double totalNetWeight;

	int totalNetSupply;

	SafetyAdvisor(void);
	~SafetyAdvisor(void);
};

