#ifndef LOCATION_H_
#define LOCATION_H_

#include <iostream>

/*
    constants
*/
const int TDIRECTIONS = 4;
enum eDirection {N, E, S, W};
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
};

std::ostream& operator<<(std::ostream &os, const Location& loc);
std::ostream& operator<<(std::ostream &os, const eDirection& e);

#endif //LOCATION_H_
