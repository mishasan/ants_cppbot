#include "Bot.h"

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

        makeMoves();
        endTurn();
    }
};

//makes the bots moves for the turn
void Bot::makeMoves()
{
    state.bug << "turn " << state.turn << ":" << endl;
    state.bug << state << endl;

    //picks out moves for each ant
    for(size_t ant = 0; ant < state.myAnts.size(); ++ant)
    {
		const Location locAnt = state.myAnts[ant];
		bool bMovedAnt = false;

		Location locClosestFood;
		bool bFoundCloseFood = state.getClosestFood(locAnt, locClosestFood);
		if(bFoundCloseFood)
		{
			AntDirection dirFood = AntDirection::N;
			if(state.getAMovingDirectionTo(locAnt, locClosestFood, dirFood))
			{
				state.makeMove(locAnt, dirFood);
				bMovedAnt = true;
			}
		}
		else
		{
			 //TODO: if there is no good direction towards food, send ant exploring?
			AntDirection dirRandom = AntDirection::N;
			if(state.getARandomDirectionFrom(locAnt, dirRandom))
			{
				state.makeMove(locAnt, dirRandom);
				bMovedAnt = true;
			}
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
