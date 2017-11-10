#pragma once

#include "Square.h"
#include "Location.h"
#include <vector>
#include <iostream>

class Ant;

class Map
{
public:
	static Map& map();

	void setDimensions(unsigned int rows, unsigned int cols);
	unsigned int rows() const { return map().m_rows; }
	unsigned int cols() const { return map().m_cols; }

	void resetCellsToLand();
	void makeMoveLocal(Ant& ant);
	bool revertLocalMove(Ant& ant);

	std::vector<Square>& operator[](const size_t& row)
	{
		return m_grid[row];
	}
	const std::vector<Square>& operator[](const size_t& row) const
	{
		return m_grid[row];
	}
	Square& operator()(const Location& loc) { return (*this)[loc.row][loc.col]; }
	const Square& operator()(const Location& loc) const { return (*this)[loc.row][loc.col]; }

	void printKnownMap(std::ostream& os);
	void printScoreMap(std::ostream& os);
	void updatePathScore();
	void calcPathScore(Location& loc);

	bool findSquareNeighborIf(const Location& loc, unsigned int neighborhoodRadius, bool (Square::*pF)() const, Location& locNgh);

private:
	Map();
	~Map();

	bool findSquareNeighborIfLine(int iMin, int iMax, int jMin, int jMax, bool (Square::*pF)() const, Location& locNgh);

	unsigned int m_rows, m_cols;

	std::vector<std::vector<Square> > m_grid;
};
