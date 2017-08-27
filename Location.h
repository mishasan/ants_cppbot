#ifndef LOCATION_H_
#define LOCATION_H_

#include <iostream>
#include <vector>

/*
    constants
*/
enum class AntDirection {N, E, S, W};
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}

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
	static const std::vector<AntDirection> getAllDirections();
};

std::ostream& operator<<(std::ostream &os, const Location& loc);
std::ostream& operator<<(std::ostream &os, const AntDirection& e);

#endif //LOCATION_H_
