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

		state.updateAntList();
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
	std::map<Location, Location> foodOrders;
	state.collectFoodOrders(foodOrders);

	for(auto& ant : state.myAnts)
	{
		Order antOrder;

		//	TODO: collect foodOrders first, set Orders in Ants second and then process still free ants
		Location locClosestFood;
		bool bFoundCloseFood = state.getClosestFood(ant, foodOrders, locClosestFood);
		if(bFoundCloseFood)
		{
			AntDirection dirFood = AntDirection::N;
			if(state.getAMovingDirectionTo(ant, locClosestFood, dirFood))
			{
				//	if another ant is taking care of that food, switch order to this ant
				auto existingFoodOrder = foodOrders.find(locClosestFood);
				if(existingFoodOrder != foodOrders.end())
				{
					const Location& locAntMoreDistant = existingFoodOrder->second;
					Ant* pDistantAnt = state.getAntByLocation(locAntMoreDistant);
					if(pDistantAnt != nullptr && !(pDistantAnt == &ant))
					{
						pDistantAnt->saveLastOrder();
					}
					existingFoodOrder->second = ant.getLocation();
					//	TODO: wenn ich der anderen Ameise den Auftrag wegnehme, muss ich ihr einen neuen geben! (jetzt: einfach bis naechste Runde warten)
				}
				else
				{
					foodOrders[locClosestFood] = ant.getLocation();
				}

				antOrder.setOrderType(Order::OrderType::Food);
				antOrder.setMove(dirFood);
				antOrder.setTarget(locClosestFood);
			}
		}
		else
		{
			//TODO: if there is no good direction towards food, send ant exploring?
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
			state.makeMoveLocal(ant);
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
