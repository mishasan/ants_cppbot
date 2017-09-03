#include "Location.h"
#include "Map.h"

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

//returns the euclidean distance between two locations with the edges wrapped
double Location::distance(const Location &loc1, const Location &loc2)
{
	unsigned int d1 = std::abs(loc1.row - loc2.row),
				 d2 = std::abs(loc1.col - loc2.col),
				 dr = std::min(d1, Map::map().rows() - d1),
				 dc = std::min(d2, Map::map().cols() - d2);
	return sqrt((dr * dr) + (dc * dc));
};

//returns the new location from moving in a given direction with the edges wrapped
Location Location::getLocation(const Location &loc, AntDirection direction)
{
	return Location( (loc.row + DIRECTIONS[(int)direction][0] + Map::map().rows()) % Map::map().rows(),
					 (loc.col + DIRECTIONS[(int)direction][1] + Map::map().cols()) % Map::map().cols() );
};

//	return the new location relative to the location, edges wrapped
Location Location::getLocationRelative(const Location &loc, int diffRow, int diffCol)
{
	return Location( (loc.row + diffRow + Map::map().rows()) % Map::map().rows(),
					 (loc.col + diffCol + Map::map().cols()) % Map::map().cols() );
}

bool Location::operator==(const Location& loc) const
{
	return this->row == loc.row && this->col == loc.col;
}

bool Location::operator<(const Location& loc) const
{
	if(this->row < loc.row || (this->row == loc.row && this->col < loc.col))
	{
		return true;		
	}
	else 
	{
		return false;
	}
}


AntDirection Location::getCounterDirection(AntDirection dir)
{
	switch (dir)
	{
		case AntDirection::N:
			return AntDirection::S;
		case AntDirection::E:
			return AntDirection::W;
		case AntDirection::S:
			return AntDirection::N;
		case AntDirection::W:
			return AntDirection::E;
		default:
			throw std::logic_error("Used an unknown AntDirection.");
	}
}
