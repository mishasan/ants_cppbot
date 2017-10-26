#include "Bot.h"
#include "Ant.h"
#include "Map.h"

using namespace std;

//	plays a single game of Ants.
void Bot::playGame()
{
    //reads the game parameters and sets the internal state up
    cin >> state;
    endTurn();

	if(!state.m_PathFinder.Init())
		return;

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

		Bug::bug() << "turn " << state.turn << ":" << endl;
		Bug::bug() << state << endl;

		issueMoves();
        makeMoves();
        endTurn();
    }
}

//	figure out what each ant is going to do, create orders and store them
//	moves ants on local grid, it will be sent to the engine later
void Bot::issueMoves()
{
	issueFood();
	
	issueExploring();
}

//	sends moves of Ants for this turn to engine
void Bot::makeMoves()
{
	for(auto& ant : state.myAnts)
	{
		if(ant.hasOrder())
		{
			state.sendMoveToEngine(ant);
		}
	}

    Bug::bug() << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
}

//	finishes the turn
void Bot::endTurn()
{
    if(state.turn > 0)
    {
		state.reset();
		state.markPreviousAnts();
	}
    state.turn++;

    cout << "go" << endl;
}

//	checks available food and sends Ants to each if reasonable 
void Bot::issueFood()
{
	std::map<Location, Location> foodOrders;

	//	find closest Ant to each food
	for(auto& ant : state.myAnts)
	{
		Location locClosestFood;
		bool bFoundCloseFood = state.getClosestFood(ant, locClosestFood); //TODO: check real distance?
		if(bFoundCloseFood)
		{
			if(!isAnotherAntCloserToThisFood(foodOrders, locClosestFood, ant))
			{
				foodOrders[locClosestFood] = ant.getLocation();
			}
		}
	}

	//	find best move to food for closest Ant
	for(const auto& foodOrder : foodOrders)
	{
		//	only food with an Ant close enough will be taken care of
		const Location& locFood = foodOrder.first;
		Ant *pAnt = getCollectingAntFor(foodOrders, locFood);
		if(pAnt == nullptr)
			continue;

		//	find best move
		Order antOrder;
		AntDirection dirFood = AntDirection::N;
		bool bFoundGoodDirection = state.getAMovingDirectionTo(*pAnt, locFood, dirFood);
		if(bFoundGoodDirection)
		{
			//	send the Ant to this food
			antOrder.setOrderType(Order::OrderType::Food);
			antOrder.setMove(dirFood);
			antOrder.setTarget(locFood);
		}

		//	move Ant locally
		pAnt->setOrder(antOrder);
		if(antOrder.getOrderType() != Order::OrderType::Idle)
		{
			Map::map().makeMoveLocal(*pAnt);
		}
	}
}

//	checks the food local orders and returns the (one) Ant which is going for this food
//	nullptr, if there is no Ant going for this food
Ant* Bot::getCollectingAntFor(std::map<Location, Location>& m_foodOrders, const Location& locFood)
{
	const auto& existingFoodOrder = m_foodOrders.find(locFood);
	if(existingFoodOrder != m_foodOrders.end())
	{
		const Location& antLoc = existingFoodOrder->second;
		for(auto& ant : state.myAnts)
		{
			if(ant.getLocation() == antLoc)
			{
				return &ant;
			}
		}
	}

	return nullptr;
}

//	checks if an ant is sent to that food already and if so, is that one is closer
bool Bot::isAnotherAntCloserToThisFood(std::map<Location, Location>& foodOrders, const Location& locFood, Ant& ant) const
{
	auto existingFoodOrder = foodOrders.find(locFood);
	if(existingFoodOrder != foodOrders.end())
	{
		const Location& otherAntForFood = existingFoodOrder->second;
		double dDistOtherAnt = Location::distance(otherAntForFood, locFood);
		double dDistAnt = Location::distance(ant.getLocation(), locFood);
		if(dDistOtherAnt < dDistAnt)
		{
			return true;
		}
	}

	return false;
}

void Bot::issueExploring()
{
	for(auto& ant : state.myAnts)
	{
		Order antOrder = ant.getOrder();

		//	only send exploring, if there is no other order
		if(antOrder.getOrderType() != Order::OrderType::Idle)
		{
			continue;
		}

		AntDirection dir = AntDirection::N;
		if(state.getAnExploringDirection(ant, dir))
			//if(state.getARandomDirectionFrom(locAnt, dir))
		{
			antOrder.setOrderType(Order::OrderType::Explore);
			antOrder.setMove(dir);
			antOrder.setTarget(Location::getLocation(ant.getLocation(), dir));
		}

		ant.setOrder(antOrder);
		if(ant.hasOrder())
		{
			Map::map().makeMoveLocal(ant);
		}
	}
}