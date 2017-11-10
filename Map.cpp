#include "Map.h"
#include "Ant.h"
#include "Square.h"
#include "Bug.h"

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
			if(!m_grid[row][col].IsWater())
				m_grid[row][col].reset();
}

void Map::makeMoveLocal(Ant& ant)
{
	const Location& oldLoc = ant.getLocation();
	Location newLoc = ant.getNewLocation();
	Bug::bug() << "makeMoveLocal: From " << oldLoc << " to " << newLoc << std::endl;

	m_grid[newLoc.row][newLoc.col].ant = m_grid[oldLoc.row][oldLoc.col].ant;
	m_grid[oldLoc.row][oldLoc.col].ant = -1;
}

//	true, if movement could be reverted
bool Map::revertLocalMove(Ant& ant)
{
	const Order& order = ant.getOrder();

	//	only a moving Order can be reversed
	if(order.getOrderType() == Order::OrderType::Idle)
		return true;

	//	check current move in Orders of Ant, determine reverse move and get old Location
	Location newLoc = ant.getNewLocation();
	AntDirection dOpposite = Location::getCounterDirection(order.getMove());
	Location oldLoc = Location::getLocation(newLoc, dOpposite);

	//	check if an Ant already moved (locally) on old Location
	int antOnOldLoc = m_grid[oldLoc.row][oldLoc.col].ant;
	if(antOnOldLoc != -1)
		return false;

	//	put Ant back to old Location
	m_grid[oldLoc.row][oldLoc.col].ant = m_grid[newLoc.row][newLoc.col].ant;
	m_grid[newLoc.row][newLoc.col].ant = -1;

	Bug::bug() << "makeMoveLocal Revert: From (new): " << newLoc << " back to (old): " << oldLoc << std::endl;

	return true;
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

//	calculating a score for each Square in the map representing closeness to water [0...1] 0 very close, 1 far away
void Map::updatePathScore()
{
	//	TODO: use spread alg to just cover areas which are visible to update that map
	//	TODO: count unfogged squares, if everything is uncovered, stop updating path score

	for(unsigned int row = 0; row < Map::map().rows(); ++row)
	{
		for(unsigned int col = 0; col < Map::map().cols(); ++col)
		{
			Location loc(row, col);
			calcPathScore(loc);
		}
	}
}

//	calcs score for a square being close to water
void Map::calcPathScore(Location& loc)
{
	Square& sq = (*this)[loc.row][loc.col];
	if(sq.pathScoreComplete)
	{
		return;
	}

	//	if square wasn't visible at least once, can't tell
	if(sq.IsFogged())
	{
		sq.pathScore = PATHSCORE_UNKNOWN;
		sq.pathScoreComplete = false;
		return;
	}

	//	cant move on water, give lowest score right away
	if(sq.IsWater())
	{
		sq.pathScore = 0;
		sq.pathScoreComplete = true;
		return;
	}

	//	size for 2D neighborhood of one square
	const int neighbsize = 1; // TODO: make global and able to adapt to maze, viewradius etc

	int iUnfoggedSquaresTotal = 0;
	int iWaterSquaresTotal = 0;
	int iLandSquaresTotal = 0;
	int iWaterDirectNgh = 0;

	for(int col = -neighbsize; col <= neighbsize; ++col)
	{
		for(int row = -neighbsize; row <= neighbsize; ++row)
		{
			const Location locRelative = Location::getLocationRelative(loc, col, row);
			Square& sqRel = (*this)[locRelative.row][locRelative.col];
			if(sqRel.IsFogged())
			{
				continue;
			}

			//	count water and land squares in neighborhood
			if(sqRel.IsWater())
			{
				iWaterSquaresTotal++;
				if((abs(col) < 2) && (abs(row) < 2))
				{
					iWaterDirectNgh++;
				}
			}
			else if(sqRel.IsLand())
			{
				iLandSquaresTotal++;
			}
			iUnfoggedSquaresTotal++;
		}
	}

	//	Score #Land to #Water - a lot of Water around makes a low Score, a lot of Land a high Score
	//	a lot of fogged squares makes a high score as well, so interesting to explore
	const int nghSize = (2 * neighbsize + 1) * (2 * neighbsize + 1);
	sq.pathScore = nghSize - iWaterDirectNgh;

	//	Do I need to check this square again because of not visible neighboring squares?
	bool bAllNeighborSquaresVisible = iUnfoggedSquaresTotal == nghSize;
	sq.pathScoreComplete = bAllNeighborSquaresVisible;
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

//	spread in circles from Start Location and check neighboring Squares for Condition
//@return first Square that meets condition
bool Map::findSquareNeighborIf(const Location& loc, unsigned int neighborhoodRadius, bool (Square::*pF)() const, Location& locNgh)
{
	const int x = loc.row, y = loc.col;
	for(unsigned int d = 1; d <= neighborhoodRadius; ++d)
	{
		if(findSquareNeighborIfLine(x - d, x + d, y - d, y - d, pF, locNgh) ||	//	top
		   findSquareNeighborIfLine(x - d, x + d, y + d, y + d, pF, locNgh) ||	//	bottom
		   findSquareNeighborIfLine(x - d, x - d, y - d + 1, y + d - 1, pF, locNgh) ||	//	left
		   findSquareNeighborIfLine(x + d, x + d, y - d + 1, y + d - 1, pF, locNgh))	//	right
			return true;
	}
	return false;
}

bool Map::findSquareNeighborIfLine(int iMin, int iMax, int jMin, int jMax, bool (Square::*pF)() const, Location& locNgh)
{
	for (int i = iMin; i <= iMax; ++i)
	{
		for (int j = jMin; j <= jMax; ++j)
		{
			locNgh = Location::getLocation(i, j);
			Square& sq = (*this)(locNgh);
			if ((sq.*pF)())
			{
				return true;
			}
		}
	}
	return false;
}