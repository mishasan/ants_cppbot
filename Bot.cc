#include "Bot.h"
#include "Ant.h"

using namespace std;

//constructor
Bot::Bot()
{

};

//plays a single game of Ants.
void Bot::playGame()
{
    //reads the game parameters and sets up
    cin >> state;
    state.setup();
    endTurn();

    //continues making moves while the game is not over
    while(cin >> state)
    {
#ifdef _DEBUG
		DeBug deBug;
		deBug.wait();
#endif // _DEBUG
	
        state.updateVisionInformation();
		state.updatePathScore();

		issueMoves();
        makeMoves();
        endTurn();
    }
};

//	figure out what each ant is going to do, create orders and store them
//	moves ants on local grid, it will be sent to the engine later
void Bot::issueMoves()
{
	//picks out moves for each ant
	for(auto& ant : state.myAnts)
	{
		const Location& locAnt = ant.getLocation();

		bool bMovedAnt = false;

		Location locClosestFood;
		bool bFoundCloseFood = state.getClosestFood(locAnt, locClosestFood);
		if(bFoundCloseFood)
		{
			AntDirection dirFood = AntDirection::N;
			if(state.getAMovingDirectionTo(locAnt, locClosestFood, dirFood))
			{
				Order order;
				order.setOrderType(Order::OrderType::Food);
				order.setMove(dirFood);
				ant.setOrder(order);
				state.makeMoveLocal(ant);
				bMovedAnt = true;
			}
		}
		else
		{
			//TODO: if there is no good direction towards food, send ant exploring?
			AntDirection dirRandom = AntDirection::N;
			if(state.getARandomDirectionFrom(locAnt, dirRandom))
			{
				Order order;
				order.setOrderType(Order::OrderType::Food);
				order.setMove(dirRandom);
				ant.setOrder(order);
				state.makeMoveLocal(ant);
				bMovedAnt = true;
			}
		}

		if(!bMovedAnt)
		{
			Order orderIdle;
			orderIdle.setOrderType(Order::OrderType::Idle);
			ant.setOrder(orderIdle);
		}
	}
}

//makes the bots moves for the turn
void Bot::makeMoves()
{
    state.bug << "turn " << state.turn << ":" << endl;
    state.bug << state << endl;

	for(auto& ant : state.myAnts)
	{
		if(ant.getOrder().getOrderType() != Order::OrderType::Idle)
		{
			state.makeMove(ant);
		}
	}

    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
};

//finishes the turn
void Bot::endTurn()
{
    if(state.turn > 0)
        state.reset();
    state.turn++;

    cout << "go" << endl;
};
