#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <map>

#include "Timer.h"
#include "Bug.h"
#include "Square.h"
#include "Location.h"
#include "PathFinder.h"

class Ant;

class State
{
public:
	int turn, turns,
        playerCount;
    double attackradius, spawnradius, viewradius;
    double loadtime, turntime;
    std::vector<double> scores;
	
    bool gameover;
    long long seed;

    std::vector<Location> enemyAnts, myHills, enemyHills, food;
	std::vector<Ant> myAnts;
	PathFinder m_PathFinder;

    Timer timer;
    
    /*
        Functions
    */
    State();
    ~State();

    void reset();
	
    void sendMoveToEngine(Ant& ant);

	bool getClosestFood(Ant& ant, Location &locClosestFood);
	bool isAnotherAntCloserToThisFood(const Location& locFood, double dDistToAnt);

	bool getAMovingDirectionTo(const Ant &ant, const Location &locTo, AntDirection& aDirection);
	bool getARandomDirectionFrom(const Location& locFrom, AntDirection& dirRandom);
	bool getAnExploringDirection(Ant& ant, AntDirection& dir);

	bool isTargetPositionFreeToGo(const Location& locTo);
	bool isAntOnPosition(const Location& locTo);
	bool isThisGoingBackwards(const Ant& ant, const AntDirection dir) const;
	bool isMoveALoop(const Ant& ant, const AntDirection dir);

    void updateVisionInformation();
	void updatePathScore();
	void calcPathScore(Location& loc);
	
	void addAnt(unsigned int row, unsigned int col, unsigned int player);
	Ant* getAntByLocation(const Location& loc);
	void markPreviousAnts();
	void updateAntList();

	void collectFoodOrders(std::map<Location, Location>& foodOrders);
	void setFoodOrderFor(const Ant& ant, const Location& locFood);
	Ant* getCollectingAntFor(Location& locFood);
	void resetFoodOrders() { m_foodOrders.clear(); }

private:
	std::map<Location, Location> m_foodOrders;
};

std::ostream& operator<<(std::ostream &os, const State &state);
std::istream& operator>>(std::istream &is, State &state);
void readTurnType(std::istream &is, State &state);
void readGameParameters(std::istream &is, State &state);
void readCurrentTurnToState(std::istream &is, State &state);

#endif //STATE_H_
