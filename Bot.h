#pragma once

#include "State.h"
#include "Orders.h"

#include <map>
#include <set>

//	MyBot 
class Bot
{
public:
	Bot() {};

	void playGame();    //plays a single game of Ants

private:
	void issueMoves();	// which ants is doing what, collect orders
	void makeMoves();   // makes moves for a single turn
	void endTurn();     //indicates to the engine that it has made its moves

	void issueFood();
	bool isAnotherAntCloserToThisFood(std::map<Location,Ant*>& foodOrders, const Location& locFood, Ant& ant) const;
	void loadPreviousFoodOrders(std::set<Ant*>& ants, std::map<Location, Ant*>& foodOrders);
	void assignBestMovesToFood(std::map<Location, Ant*>& foodOrders);
	void findClosestFoodToAnts(std::set<Ant*>& antsForFood, std::map<Location, Ant*>& foodOrders);

	void issueExploring();
 
	State state;
};