#include "State.h"

using namespace std;
#include <random>
#include <queue>
#include <map>
#include <array>
#include "Ant.h"
#include "Map.h"

#ifndef _DEBUG
#include <chrono>
#endif
#include <sstream>
#include "PathFinder.h"

//constructor
State::State()
{
    gameover = 0;
    turn = 0;
    Bug::bug().open("./debug.txt");
};

//deconstructor
State::~State()
{
    Bug::bug().close();
};

// clears the bots ant vector and resets cells
void State::reset()
{
    //myAnts.clear();
    enemyAnts.clear();
    myHills.clear();
    enemyHills.clear();
    food.clear();
    Map::map().resetCellsToLand();
}

void State::sendMoveToEngine(Ant& ant)
{
	const Location& loc = ant.getLocation();
	const AntDirection moveDirection = ant.getOrder().getMove();
    cout << "o " << loc.row << " " << loc.col << " " << moveDirection << endl;
	Bug::bug() << "o " << loc.row << " " << loc.col << " " << moveDirection << endl;

#if _DEBUG
	Bug::bug() << "sendMoveToEngine " << ant.print() << endl;
#endif
	ant.moveByOrder();
}

bool State::getAMovingDirectionTo(const Ant &ant, const Location &locTo, AntDirection& aDirection)
{
	vector<AntDirection> path;
	if(!m_PathFinder.findPath(ant.getLocation(), locTo, path))
		return false;

	AntDirection dNextMove = *path.begin();

	const Location locTestDirection = Location::getLocation(ant.getLocation(), dNextMove);
	if(!isTargetPositionFreeToGo(locTestDirection))
	{
		return false;
	}

	aDirection = dNextMove;

	return true;
}

//	returns true and a random direction with a valid target location, or false
bool State::getARandomDirectionFrom(const Location& locFrom, AntDirection& dirRandom)
{
	vector<AntDirection> vAllDirections(AllAntDirections.begin(), AllAntDirections.end());
#ifdef _DEBUG
	unsigned int ranseed = (unsigned int)seed; 
#else
	unsigned int ranseed = std::chrono::system_clock::now().time_since_epoch().count();
#endif // _DEBUG
	std::shuffle(vAllDirections.begin(), vAllDirections.end(), std::default_random_engine(ranseed));
	for(auto dir : vAllDirections)
	{
		Location locTo = Location::getLocation(locFrom, dir);
		if(isTargetPositionFreeToGo(locTo))
		{
			dirRandom = dir;
			return true;
		}
	}

	return false;
}

bool State::getAnExploringDirection(Ant& ant, AntDirection& dirExploreTo)
{
	//	sort possible Directions by Score, low to high
	std::multimap<int, AntDirection> mapDirByScore;
	for(auto dir : AllAntDirections)
	{
		Location loc = Location::getLocation(ant.getLocation(), dir);
		if(isTargetPositionFreeToGo(loc))
		{
			const Square& sq = Map::map()[loc.row][loc.col];
			mapDirByScore.insert(make_pair(sq.pathScore, dir));
		}
	}

	//	don't move if there is no neighboring position available
	if(mapDirByScore.empty())
	{
		return false;
	}

	//	picks out of available directions that one with the best score
	for(std::multimap<int, AntDirection>::reverse_iterator itScoreHighToLow = mapDirByScore.rbegin();
		itScoreHighToLow != mapDirByScore.rend(); ++itScoreHighToLow)
	{
		AntDirection dir = itScoreHighToLow->second;

		//	TODO: for now the ant is not allowed to go back where it came from -> could end up in trapped ants
		//if(isThisGoingBackwards(ant, dir))
		if(isMoveALoop(ant, dir))
		{
			continue;
		}
		else
		{
			dirExploreTo = dir;
			return true;
		}
	}

	return false;
}

bool State::isTargetPositionFreeToGo(const Location& locTo)
{
	const Square& sqTo = Map::map()[locTo.row][locTo.col];
	if(sqTo.IsWater() || isAntOnPosition(locTo) || sqTo.IsHill())
	{
		return false;
	}

	return true;
}

bool State::isAntOnPosition(const Location& loc)
{
	const Square& sqTo = Map::map()[loc.row][loc.col];
	return sqTo.ant >= 0;
}

bool State::isThisGoingBackwards(const Ant& ant, const AntDirection dir) const
{
	AntDirection dirPreviousMove = AntDirection::N;
	return ant.getPreviousMove(dirPreviousMove) && dir == Location::getCounterDirection(dirPreviousMove);
}

bool State::getClosestFood(Ant& ant, Location &locClosestFood)
{
	const Location& locAnt = ant.getLocation();

	//	if there is non, they have to go explore
	if (food.empty())
	{
		Bug::bug() << "no food found from pos" << ant.getLocation() << endl;
		return false;
	}

	//	find close food sorted by distance to Location
	const double dMaxDistanceToFood = 2 * viewradius;
	double dMinDist = std::numeric_limits<double>::max();
	std::vector<Location>::iterator itClosestFood = food.end();
	for(std::vector<Location>::iterator itFood = food.begin(); itFood != food.end(); ++itFood)
	{
		//	TODO: use path finding mechanism to find true distance
		const double dDistToAnt = Location::distance(locAnt, *itFood);

		//	only go to food thats not too far away
		if(dDistToAnt > dMaxDistanceToFood)
			continue;

		//	remember closest food
		if(dDistToAnt < dMinDist)
		{
			itClosestFood = itFood;
			dMinDist = dDistToAnt;
		}
	}
	
	if(itClosestFood == food.end())
	{
		Bug::bug() << "no food found thats close enough to ant at " << locAnt << endl;
		return false;
	}
	else
	{
		locClosestFood = *itClosestFood;
		return true;
	}
}

//	calculating a score for each Square in the map representing closeness to water [0...1] 0 very close, 1 far away
void State::updatePathScore()
{
	//	TODO: use spread alg to just cover areas which are visible to update that map
	//	TODO: count unfogged squares, if everything is uncovered, stop updating path score

	for(unsigned int row = 0; row < Map::map().rows(); ++row)
	{
		for(unsigned int col = 0; col < Map::map().cols(); ++col)
		{
			Location loc(row, col);
			calcPathScore(loc);
		}
	}
}

//	calcs score for a square being close to water
void State::calcPathScore(Location& loc)
{
	Square& sq = Map::map()[loc.row][loc.col];
	if(sq.pathScoreComplete)
	{
		return;
	}

	//	if square wasn't visible at least once, can't tell
	if(sq.isFogged())
	{
		sq.pathScore = PATHSCORE_UNKNOWN;
		sq.pathScoreComplete = false;
		return;
	}

	//	cant move on water, give lowest score right away
	if(sq.IsWater())
	{
		sq.pathScore = 0;
		sq.pathScoreComplete = true;
		return;
	}

	//	size for 2D neighborhood of one square
	const int neighbsize = 1; // TODO: make global and able to adapt to maze, viewradius etc

	int iUnfoggedSquaresTotal = 0;
	int iWaterSquaresTotal = 0;
	int iLandSquaresTotal = 0;
	int iWaterDirectNgh = 0;
	
	for(int col = -neighbsize; col <= neighbsize; ++col)
	{
		for(int row = -neighbsize; row <= neighbsize; ++row)
		{
			const Location locRelative = Location::getLocationRelative(loc, col, row);
			Square& sqRel = Map::map()[locRelative.row][locRelative.col];
			if(sqRel.isFogged())
			{
				continue;
			}

			//	count water and land squares in neighborhood
			if(sqRel.IsWater())
			{
				iWaterSquaresTotal++;
				if((abs(col) < 2) && (abs(row) < 2))
				{
					iWaterDirectNgh++;
				}
			}
			else if(sqRel.IsLand())
			{
				iLandSquaresTotal++;
			}
			iUnfoggedSquaresTotal++;
		}
	}

	//	Score #Land to #Water - a lot of Water around makes a low Score, a lot of Land a high Score
	//	a lot of fogged squares makes a high score as well, so interesting to explore
	const int nghSize = (2 * neighbsize + 1) * (2 * neighbsize + 1);
	sq.pathScore = nghSize - iWaterDirectNgh;
	
	//	Do I need to check this square again because of not visible neighboring squares?
	bool bAllNeighborSquaresVisible = iUnfoggedSquaresTotal == nghSize;
	sq.pathScoreComplete = bAllNeighborSquaresVisible;
}

/*
    This function will update update the lastSeen value for any squares currently
    visible by one of your live ants.

    BE VERY CAREFUL IF YOU ARE GOING TO TRY AND MAKE THIS FUNCTION MORE EFFICIENT,
    THE OBVIOUS WAY OF TRYING TO IMPROVE IT BREAKS USING THE EUCLIDEAN METRIC, FOR
    A CORRECT MORE EFFICIENT IMPLEMENTATION, TAKE A LOOK AT THE GET_VISION FUNCTION
    IN ANTS.PY ON THE CONTESTS GITHUB PAGE.
*/
void State::updateVisionInformation()
{
    for(const auto& ant : myAnts)
    {
		const Location& antLoc = ant.getLocation();
        std::queue<Location> locQueue;
		locQueue.push(antLoc);

        std::vector<std::vector<bool> > visited(Map::map().rows(), std::vector<bool>(Map::map().cols(), 0));
        Map::map()[antLoc.row][antLoc.col].SetVisible();
        visited[antLoc.row][antLoc.col] = 1;

        while(!locQueue.empty())
        {
            const Location curLoc = locQueue.front();
            locQueue.pop();
						
			for(auto dir : AllAntDirections)
			{
                const Location nLoc = Location::getLocation(curLoc, dir);
                if(!visited[nLoc.row][nLoc.col] && Location::distance(antLoc, nLoc) <= viewradius)
                {
					Square& nSq = Map::map()[nLoc.row][nLoc.col];
                    nSq.SetVisible();
					nSq.SetToLand(!nSq.IsWater());	//all visible squares, that aren't water are land and unfogged
                    locQueue.push(nLoc);
                }
                visited[nLoc.row][nLoc.col] = 1;
            }
        }
    }
};

void State::addAnt(unsigned int row, unsigned int col, unsigned int antPlayerNr)
{
	Map::map()[row][col].ant = antPlayerNr;

	const unsigned int playerNrMe = 0;
	Location antLoc(row, col);
	if(antPlayerNr == playerNrMe)
	{
		//	check if Ant is already in list
		Ant* pAnt = getAntByLocation(antLoc);
		if(pAnt != nullptr)
		{
			pAnt->saveLastOrder();

			//	mark existing Ant as still available in this turn
			pAnt->setValid();
		}
		else
		{
			myAnts.push_back(Ant(antLoc));
		}
		
		//	at end, remove dead ants
	}
	else
	{
		enemyAnts.push_back(antLoc);
	}
}

Ant* State::getAntByLocation(const Location& loc)
{
	std::vector<Ant>::iterator itFnd = std::find_if(myAnts.begin(), myAnts.end(), [loc] (Ant& ant) { return ant.getLocation() == loc; } );
	if(itFnd != myAnts.end())
	{
		return &(*itFnd);
	}
	else
	{
		return nullptr;
	}
}

void State::markPreviousAnts()
{
	for(auto& ant : myAnts)
	{
		ant.setValid(false);
	}
}

void State::updateAntList()
{
	//	just keeps Ants in the list, that got send again by the engine
	myAnts.erase(std::remove_if(myAnts.begin(), myAnts.end(), [] (Ant& ant) { return !ant.isValid(); }), myAnts.end());

	// TODO: collect invalid ants as dead ants for gathering information about fights or movement errors
}

//	checks a movement if it is making the ant moving in circles
bool State::isMoveALoop(const Ant& ant, const AntDirection dir)
{
	std::vector<AntDirection> prevMoves;
	ant.getLastMoves(prevMoves);

	if(prevMoves.size() < 2)
	{
		return false;
	}

	//	add coordinate-changing part of Directions (out of XY-Values) 
	//	to check if all movements cancel each other and the ant ends up at the same place
	array<int, 2> dirSums = {0, 0}; // NS, EW
	const int maxmoves = 10;
	for(size_t d = 0; d < maxmoves && d < prevMoves.size(); ++d)
	{
		const AntDirection& dirPrev = prevMoves[d];
		switch (dirPrev)
		{
			case AntDirection::E:
				dirSums[1] += 1;
				break;
			case AntDirection::W:
				dirSums[1] -= 1;
				break;
			case AntDirection::N:
				dirSums[0] -= 1;
				break;
			case AntDirection::S:
				dirSums[0] += 1;
			default:
				break;
		}

		bool dirSumsCancelOut = dirSums[0] == 0 && dirSums[1] == 0;

		//	break, if movements cancel each other out and the planned movement is equal to the first of that circle
		if(dirSumsCancelOut)
		{
			return dir == dirPrev;
		}
 	}

	return false;
}

/*
    This is the output function for a state. It will add a char map
    representation of the state to the output stream passed to it.

    For example, you might call "cout << state << endl;"
*/
ostream& operator<<(ostream &os, const State &state)
{
	Map::map().printKnownMap(os);
	Map::map().printScoreMap(os);
    return os;
};

//input function
istream& operator>>(istream &is, State &state)
{
	readTurnType(is, state);

    if(state.turn == 0)
    {
        readGameParameters(is, state);
    }
    else
    {
        readCurrentTurnToState(is, state);
    }

    return is;
}

//finds out which turn it is
void readTurnType(istream &is, State &state)
{
	string inputType;
    while(is >> inputType)
    {
        if(inputType == "end")
        {
            state.gameover = 1;
            break;
        }
        else if(inputType == "turn")
        {
            is >> state.turn;
            break;
        }
        else
		{
			//unknown line
			string junk;
	        getline(is, junk);
		}
    }
}

//reads game parameters
void readGameParameters(istream &is, State &state)
{
	string inputType;
	unsigned int rows = 0, cols = 0;
    while(is >> inputType)
    {
        if(inputType == "loadtime")
            is >> state.loadtime;
        else if(inputType == "turntime")
            is >> state.turntime;
        else if(inputType == "rows")
            is >> rows;
        else if(inputType == "cols")
            is >> cols;
        else if(inputType == "turns")
            is >> state.turns;
        else if(inputType == "player_seed")
            is >> state.seed;
        else if(inputType == "viewradius2")
        {
            is >> state.viewradius;
            state.viewradius = sqrt(state.viewradius);
        }
        else if(inputType == "attackradius2")
        {
            is >> state.attackradius;
            state.attackradius = sqrt(state.attackradius);
        }
        else if(inputType == "spawnradius2")
        {
            is >> state.spawnradius;
            state.spawnradius = sqrt(state.spawnradius);
        }
        else if(inputType == "ready") //end of parameter input
        {
            state.timer.start();
            break;
        }
        else
		{
			//unknown line
			string junk;
			getline(is, junk);
		}
    }

	Map::map().setDimensions(rows, cols);
}

void readCurrentTurnToState(istream &is, State &state)
{
	string inputType, junk;
	unsigned int row, col, player;
	while(is >> inputType)
    {
        if(inputType == "w") //water square
        {
            is >> row >> col;
            Map::map()[row][col].SetToWater();
        }
        else if(inputType == "f") //food square
        {
            is >> row >> col;
            Map::map()[row][col].SetFood();
            state.food.push_back(Location(row, col));
        }
        else if(inputType == "a") //live ant square
        {
            is >> row >> col >> player;
			state.addAnt(row, col, player);
        }
        else if(inputType == "d") //dead ant square
        {
            is >> row >> col >> player;
            Map::map()[row][col].deadAnts.push_back(player);
        }
        else if(inputType == "h")	//hill square
        {
            is >> row >> col >> player;
            Map::map()[row][col].SetToHill(player);
            if(player == 0)
                state.myHills.push_back(Location(row, col));
            else
                state.enemyHills.push_back(Location(row, col));

        }
        else if(inputType == "players") //player information
            is >> state.playerCount;
        else if(inputType == "scores") //score information
        {
            state.scores = vector<double>(state.playerCount, 0.0);
            for(int p=0; p<state.playerCount; p++)
                is >> state.scores[p];
        }
        else if(inputType == "go") //end of turn input
        {
            if(state.gameover)
                is.setstate(std::ios::failbit);
            else
                state.timer.start();
            break;
        }
        else //unknown line
            getline(is, junk);
	}
}