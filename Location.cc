#include "Location.h"

std::ostream& operator<<(std::ostream &os, const Location& loc)
{
	os << "[" << loc.row << ", " << loc.col << "]";
	return os;
}


std::ostream& operator<<(std::ostream &os, const AntDirection& e)
{
	unsigned int iE = static_cast<unsigned int>(e);
	static const unsigned int iDirCount = 4;
	if(iE < iDirCount)
	{
		static const char cDirections[iDirCount] = {'N', 'E', 'S', 'W'};
		os << cDirections[(size_t)e];
	}
	return os;
}

const std::vector<AntDirection> Location::getAllDirections()
{
	AntDirection dirs[] = {AntDirection::N, AntDirection::E, AntDirection::S, AntDirection::W};
	return std::vector<AntDirection> (dirs, dirs + 4);	//VS compliant initialization by array values
}
