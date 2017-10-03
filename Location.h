#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <utility>

/*
    constants
*/
enum class AntDirection {N, E, S, W};
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}
static const std::array<std::pair<int, int>, 4> AllAntDirectionsVec = { std::make_pair(-1, 0), std::make_pair(0, 1), std::make_pair(1, 0), std::make_pair(0, -1) };
static const std::array<AntDirection, 4> AllAntDirections = { AntDirection::N, AntDirection::E, AntDirection::S, AntDirection::W };


/*
    struct for representing locations in the grid.
*/
struct Location
{
    int row, col;

    Location()
    {
        row = col = 0;
    };

    Location(int r, int c)
    {
        row = r;
        col = c;
    };

	bool operator==(const Location& loc) const;
	bool operator!=(const Location& loc) const;
	bool operator<(const Location& loc) const;

	static double distance(const Location &loc1, const Location &loc2);
	static Location getLocation(const Location &loc, AntDirection direction);
	static Location getLocationRelative(const Location &loc, int diffRow, int diffCol);
	static AntDirection getCounterDirection(AntDirection dir);
};

std::ostream& operator<<(std::ostream &os, const Location& loc);
std::ostream& operator<<(std::ostream &os, const AntDirection& e);