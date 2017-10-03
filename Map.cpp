#include "Map.h"
#include "Ant.h"
#include "Square.h"

using namespace std;

Map::Map()
{

}

Map::~Map()
{

}

Map& Map::map()
{
	static Map mapInstance;
	return mapInstance;
}

void Map::setDimensions(unsigned int rows, unsigned int cols)
{
	m_rows = rows; 
	m_cols = cols;

	m_grid = vector<vector<Square> >(m_rows, vector<Square>(m_cols, Square()));
}

//resets all non-water squares to land
void Map::resetCellsToLand()
{
	for(unsigned int row = 0; row < Map::map().rows(); row++)
		for(unsigned int col = 0; col < Map::map().cols(); col++)
			if(!m_grid[row][col].isWater)
				m_grid[row][col].reset();
}

void Map::makeMoveLocal(Ant& ant)
{
	const Location& oldLoc = ant.getLocation();
	Location newLoc = ant.getNewLocation();
	//bug << "makeMoveLocal: From " << oldLoc << " to " << newLoc << " Order: " << ant.getOrder().print() << endl;

	m_grid[newLoc.row][newLoc.col].ant = m_grid[oldLoc.row][oldLoc.col].ant;
	m_grid[oldLoc.row][oldLoc.col].ant = -1;
}

void Map::printKnownMap(ostream& os)
{
	for(unsigned int row=0; row < Map::map().rows(); row++)
	{
		for(unsigned int col=0; col < Map::map().cols(); col++)
		{
			os << Map::map()[row][col];
		}
		os << endl;
	}
	os << endl;
}

void Map::printScoreMap(std::ostream& os)
{
	for(unsigned int row=0; row < m_rows; row++)
	{
		for(unsigned int col=0; col < m_cols; col++)
		{
			const int& score = m_grid[row][col].pathScore;
			if(score < 0)
			{
				os << '?';
			}
			else
			{
				os << score;
			}
		}
		os << endl;
	}
	os << endl;
}
