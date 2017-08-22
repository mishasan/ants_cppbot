#include "Location.h"

std::ostream& operator<<(std::ostream &os, const Location& loc)
{
	os << "[" << loc.row << ", " << loc.col << "]";
	return os;
}