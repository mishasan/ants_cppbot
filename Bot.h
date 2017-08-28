#pragma once

#include "State.h"
#include "Orders.h"

//	MyBot 
class Bot
{
public:
	Bot();

	void playGame();    //plays a single game of Ants

	void issueMoves();	// which ants is doing what, collect orders
	void makeMoves();   // makes moves for a single turn
	
	void endTurn();     //indicates to the engine that it has made its moves
	
	State state;
	Orders m_orders;
};