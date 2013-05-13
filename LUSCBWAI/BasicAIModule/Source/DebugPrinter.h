#pragma once
#include <BWAPI.h>
#include <string>

class DebugPrinter
{
public:
	DebugPrinter(void);
	~DebugPrinter(void);

	static void printDebug(const char * c);
};

