#pragma once

#include "State.h"
#include "Orders.h"

//	MyBot 
class Bot
{
public:
	Bot() {};

	void playGame();    //plays a single game of Ants

	void issueMoves();	// which ants is doing what, collect orders

	void issueExploring();

	void issueFood();

	void makeMoves();   // makes moves for a single turn
	
	void endTurn();     //indicates to the engine that it has made its moves
	
	State state;

	Ant* getCollectingAntFor(std::map<Location, Location>& foodOrders, const Location& locFood);
};