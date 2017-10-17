#pragma once

#include <vector>
#include <iostream>

/*
    struct for representing a square in the grid.
*/
const int PATHSCORE_UNKNOWN = -1;

struct Square
{
    bool IsVisible() const { return isVisible; }
	void SetVisible(bool visible = true) { isVisible = visible; }
	bool IsFood() const { return isFood; }
	void SetFood(bool food = true) { isFood = food; }
	bool IsHill() const { return isHill; }
	void SetToHill(int playerNr) { isHill = true; hillPlayer = playerNr; }
	void SetToLand(bool bLand = true) { isLand = bLand; isWater = !isLand; }
	void SetToWater(bool bWater = true) { isWater = bWater; isLand = !bWater; }
	bool IsWater() const { return isWater; }
	bool IsLand() const { return isLand; }

    Square();

    //resets the information for the square except water information
    void reset();

	bool isFogged();

	char toChar() const;

	int ant, hillPlayer;
	std::vector<int> deadAnts;

	int pathScore;
	bool pathScoreComplete;

private:
	bool isWater, isLand;
	bool isVisible, isHill, isFood;
};

std::ostream& operator<<(std::ostream &os, const Square& sq);