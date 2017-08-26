#include "Location.h"

std::ostream& operator<<(std::ostream &os, const Location& loc)
{
	os << "[" << loc.row << ", " << loc.col << "]";
	return os;
}

static const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};
std::ostream& operator<<(std::ostream &os, const eDirection& e)
{
	os << CDIRECTIONS[e];
	return os;
}