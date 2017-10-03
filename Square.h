#pragma once

#include <vector>
#include <iostream>

/*
    struct for representing a square in the grid.
*/
const int PATHSCORE_UNKNOWN = -1;

struct Square
{
    bool isVisible, isWater, isHill, isFood, isLand;
    int ant, hillPlayer;
    std::vector<int> deadAnts;

	int pathScore;
	bool pathScoreComplete;

    Square();

    //resets the information for the square except water information
    void reset();

	bool isFogged();

	char toChar() const;
};

std::ostream& operator<<(std::ostream &os, const Square& sq);