#ifndef SQUARE_H_
#define SQUARE_H_

#include <vector>

/*
    struct for representing a square in the grid.
*/
const float PATHSCORE_UNKNOWN = -1.0f;

struct Square
{
    bool isVisible, isWater, isHill, isFood, isLand;
    int ant, hillPlayer;
    std::vector<int> deadAnts;

	float pathScore;
	bool pathScoreComplete;

    Square()
    {
        isVisible = isWater = isHill = isFood = isLand = false;
        ant = hillPlayer = -1;
		pathScore = PATHSCORE_UNKNOWN;
		pathScoreComplete = false;
    };

    //resets the information for the square except water information
    void reset()
    {
        isVisible = false;
        isHill = false;
        isFood = false;
        ant = hillPlayer = -1;
        deadAnts.clear();
    };

	bool isFogged()
	{
		return !(isWater || isLand);
	}
};

#endif //SQUARE_H_
