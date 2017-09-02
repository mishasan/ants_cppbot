#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>

#include "Timer.h"
#include "Bug.h"
#include "Square.h"
#include "Location.h"

class Ant;

/*
    struct to store current state information
*/
struct State
{
    /*
        Variables
    */
    int turn, turns,
        playerCount;
    double attackradius, spawnradius, viewradius;
    double loadtime, turntime;
    std::vector<double> scores;
	
    bool gameover;
    long long seed;

    std::vector<std::vector<Square> > grid;
    std::vector<Location> enemyAnts, myHills, enemyHills, food;
	std::vector<Ant> myAnts;

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

    void sendMoveToEngine(Ant& ant);
	void makeMoveLocal(Ant& ant);
 	bool getClosestFood(const Location &locFrom, Location &locClosestFood);
	bool getAMovingDirectionTo(const Location &locFrom, const Location &locTo, AntDirection& aDirection);
	bool getARandomDirectionFrom(const Location& locFrom, AntDirection& dirRandom);
	bool isTargetPositionFreeToGo(const Location& locTo);
	bool isAntOnPosition(const Location& locTo);
	bool isThisGoingBackwards(const Ant& ant, const AntDirection dir) const;

    void updateVisionInformation();
	void updatePathScore();
	void calcPathScore(Location& loc);
	void addAnt(unsigned int row, unsigned int col, unsigned int player);
	void markPreviousAnts();
	void updateAntList();
	bool getAnExploringDirection(Ant& ant, AntDirection& dir);
};

std::ostream& operator<<(std::ostream &os, const State &state);
std::istream& operator>>(std::istream &is, State &state);
void readTurnType(std::istream &is, State &state);
void readGameParameters(std::istream &is, State &state);
void readCurrentTurnToState(std::istream &is, State &state);
void printKnownMap(std::ostream& os, const State& state);
void printScoreMap(std::ostream& os, const State& state);

#endif //STATE_H_
