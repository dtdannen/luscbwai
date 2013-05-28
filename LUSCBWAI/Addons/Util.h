#include <RectangleArray.h>
#include <map>

// macro for converting ints to strings
#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
	( std::ostringstream() << std::dec << x ) ).str()

void log(const char* text, ...);
std::map<int, int> computeAssignments(Util::RectangleArray< double> &cost);


