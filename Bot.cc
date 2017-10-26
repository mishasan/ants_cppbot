#include "Bot.h"
#include "Ant.h"
#include "Map.h"

#include <set>

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
	//	create list of not assigned ants
	std::set<Ant*> antsForFood;
	for(Ant& pAnt : state.myAnts)
		antsForFood.insert(&pAnt);
	 
	std::map<Location, Ant*> foodOrders;

	loadPreviousFoodOrders(antsForFood, foodOrders);

	findClosestFoodToAnts(antsForFood, foodOrders);

	assignBestMovesToFood(foodOrders);
}

//	checks if an ant is sent to that food already and if so, is that one is closer
bool Bot::isAnotherAntCloserToThisFood(std::map<Location,Ant*>& foodOrders, const Location& locFood, Ant& ant) const
{
	auto existingFoodOrder = foodOrders.find(locFood);
	if(existingFoodOrder != foodOrders.end())
	{
		const Ant* pOtherAntForFood = existingFoodOrder->second;
		double dDistOtherAnt = Location::distance(pOtherAntForFood->getLocation(), locFood);
		double dDistAnt = Location::distance(ant.getLocation(), locFood);
		if(dDistOtherAnt < dDistAnt)
		{
			return true;
		}
	}

	return false;
}

//	checks Food Order of Ants from previous turn and matches them to still available food
void Bot::loadPreviousFoodOrders(std::set<Ant*>& ants, std::map<Location, Ant*>& foodOrders)
{
	std::vector<Ant*> vUsedAnts;
	for(Ant* pAnt : ants)
	{
		const Order& antLastOrder = pAnt->getLastOrder();
		if(pAnt->hasOrder() || antLastOrder.getOrderType() != Order::OrderType::Food)
		{
			continue;
		}
		const Location& lastTarget = antLastOrder.getTarget();
		vector<Location>::iterator itFoodFnd = find_if(state.food.begin(), state.food.end(),
												[&lastTarget](Location& locFood) {return locFood == lastTarget; });
		if(itFoodFnd != state.food.end())
		{
			foodOrders[lastTarget] = pAnt;
			vUsedAnts.push_back(pAnt);
			state.food.erase(itFoodFnd);
		}
	}

	for(Ant* pAnt : vUsedAnts)
	{
		ants.erase(pAnt);
	}
}

//	find closest food for not assigned Ants
void Bot::findClosestFoodToAnts(std::set<Ant*>& ants, std::map<Location, Ant*>& foodOrders)
{
	vector<Ant*> vUsedAnts;
	for(Ant* pAnt : ants)
	{
		if(pAnt->hasOrder())
			continue;

		Location locClosestFood;
		bool bFoundCloseFood = state.getClosestFood(*pAnt, locClosestFood);
		if(bFoundCloseFood)
		{
			if(!isAnotherAntCloserToThisFood(foodOrders, locClosestFood, *pAnt))
			{
				foodOrders[locClosestFood] = pAnt;
				vUsedAnts.push_back(pAnt);
			}
		}
	}

	for(Ant* pAnt : vUsedAnts)
	{
		ants.erase(pAnt);
	}
}

//	find best move to food for closest Ant
void Bot::assignBestMovesToFood(std::map<Location, Ant*>& foodOrders)
{
	for(const auto& foodOrder : foodOrders)
	{
		Ant *pAnt = foodOrder.second;

		//	find best move
		Order antOrder;
		AntDirection dirFood = AntDirection::N;
		const Location& locFood = foodOrder.first;
		bool bFoundGoodDirection = state.getAMovingDirectionTo(*pAnt, locFood, dirFood);
		if(bFoundGoodDirection)
		{
			//	send Ant to this food
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
