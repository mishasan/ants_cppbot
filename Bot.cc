#include "Bot.h"
#include "Ant.h"
#include "Map.h"

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
    endTurn();

    //continues making moves while the game is not over
    while(cin >> state)
    {
#ifdef _DEBUG
		DeBug deBug;
		deBug.wait();
#endif // _DEBUG

		state.updateAntList();
        state.updateVisionInformation();
		state.updatePathScore();

		state.bug << "turn " << state.turn << ":" << endl;
		state.bug << state << endl;

		issueMoves();
        makeMoves();
        endTurn();
    }
};

//	figure out what each ant is going to do, create orders and store them
//	moves ants on local grid, it will be sent to the engine later
void Bot::issueMoves()
{
	std::map<Location, Location> foodOrders;
	state.collectFoodOrders(foodOrders);

	//picks out moves for each ant
	for(auto& ant : state.myAnts)
	{
		Order antOrder;

		//	try to place a food order for this ant
		Location locClosestFood;
		bool bFoundCloseFood = state.getClosestFood(ant, foodOrders, locClosestFood);
		if(bFoundCloseFood)
		{
			auto existingFoodOrder = foodOrders.find(locClosestFood);
			if(existingFoodOrder == foodOrders.end())
			{
				AntDirection dirFood = AntDirection::N;
				bool bFoundGoodDirection = state.getAMovingDirectionTo(ant, locClosestFood, dirFood);
				if(bFoundGoodDirection)
				{
					foodOrders[locClosestFood] = ant.getLocation();
					antOrder.setOrderType(Order::OrderType::Food);
					antOrder.setMove(dirFood);
					antOrder.setTarget(locClosestFood);
				}
			}
		}

		//	no food order means, go explore
		if(antOrder.getOrderType() == Order::OrderType::Idle)
		{
			AntDirection dir = AntDirection::N;
			if(state.getAnExploringDirection(ant, dir))
			//if(state.getARandomDirectionFrom(locAnt, dir))
			{
				antOrder.setOrderType(Order::OrderType::Explore);
				antOrder.setMove(dir);
				antOrder.setTarget(Location::getLocation(ant.getLocation(), dir));
			}
		}

		ant.setOrder(antOrder);
		if(antOrder.getOrderType() != Order::OrderType::Idle)
		{
			Map::map().makeMoveLocal(ant);
		}
	}
}

//makes the bots moves for the turn
void Bot::makeMoves()
{
	for(auto& ant : state.myAnts)
	{
		if(ant.getOrder().getOrderType() != Order::OrderType::Idle)
		{
			state.sendMoveToEngine(ant);
		}
	}

    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
};

//finishes the turn
void Bot::endTurn()
{
    if(state.turn > 0)
    {
		state.reset();
		state.markPreviousAnts();
	}
    state.turn++;

    cout << "go" << endl;
};
