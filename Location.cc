#include "Location.h"

std::ostream& operator<<(std::ostream &os, const Location& loc)
{
	os << "[" << loc.row << ", " << loc.col << "]";
	return os;
}


std::ostream& operator<<(std::ostream &os, const AntDirection& e)
{
	static const char cDirections[4] = {'N', 'E', 'S', 'W'};
	os << cDirections[(int)e];
	return os;
}

const std::vector<AntDirection> Location::getAllDirections()
{
	AntDirection dirs[] = {AntDirection::N, AntDirection::E, AntDirection::S, AntDirection::W};
	return std::vector<AntDirection> (dirs, dirs + 4);	//VS compliant initialization by array values
}
