#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <stack>

#include "Timer.h"
#include "Bug.h"
#include "Square.h"
#include "Location.h"

/*
    constants
*/
const int TDIRECTIONS = 4;
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}

/*
    struct to store current state information
*/
struct State
{
    /*
        Variables
    */
    int rows, cols,
        turn, turns,
        playerCount;
    double attackradius, spawnradius, viewradius;
    double loadtime, turntime;
    std::vector<double> scores;
	
    bool gameover;
    long long seed;

    std::vector<std::vector<Square> > grid;
    std::vector<Location> myAnts, enemyAnts, myHills, enemyHills, food;

    Timer timer;
    Bug bug;

    /*
        Functions
    */
    State();
    ~State();

    void setup();
    void reset();
	void resetCellsToLand();

    void makeMove(const Location &loc, int direction);

    double distance(const Location &loc1, const Location &loc2);
    Location getLocation(const Location &startLoc, int direction);
	Location getLocationRelative(const Location &loc, int diffRow, int diffCol);
	bool getClosestFood(const Location &locFrom, Location &locClosestFood);
	bool getAMovingDirectionTo(const Location &locFrom, const Location &locTo, int& aDirection);
	bool isTargetPositionFreeToGo(const Location& locTo);
	bool isAntOnPosition(const Location& locTo);

    void updateVisionInformation();
	void updatePathScore();
	void calcPathScore(Location& loc);
};

std::ostream& operator<<(std::ostream &os, const State &state);
std::istream& operator>>(std::istream &is, State &state);
void readTurnType(std::istream &is, State &state);
void readGameParameters(std::istream &is, State &state);
void readCurrentTurnToState(std::istream &is, State &state);
void printKnownMap(std::ostream& os, const State& state);
void printScoreMap(std::ostream& os, const State& state);

#endif //STATE_H_
