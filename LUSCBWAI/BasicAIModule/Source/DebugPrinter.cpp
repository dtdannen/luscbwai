#include "DebugPrinter.h"
using namespace BWAPI;

DebugPrinter::DebugPrinter(void)
{
}


DebugPrinter::~DebugPrinter(void)
{
}

void DebugPrinter::printDebug(const char * c)
{
#ifdef RDEBUG
	Broodwar->sendText(c);
#endif
}
