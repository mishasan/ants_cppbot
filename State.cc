#include "State.h"

using namespace std;
#include <random>
#include <queue>
#include "Ant.h"
#include "Map.h"

#ifndef _DEBUG
#include <chrono>
#endif

//constructor
State::State()
{
    gameover = 0;
    turn = 0;
    bug.open("./debug.txt");
};

//deconstructor
State::~State()
{
    bug.close();
};

//sets the state up
void State::setup()
{
    grid = vector<vector<Square> >(Map::map().rows(), vector<Square>(Map::map().cols(), Square()));
};

// clears the bots ant vector and resets cells
void State::reset()
{
    myAnts.clear();
    enemyAnts.clear();
    myHills.clear();
    enemyHills.clear();
    food.clear();
    resetCellsToLand();
};

//resets all non-water squares to land
void State::resetCellsToLand()
{
	for(unsigned int row = 0; row < Map::map().rows(); row++)
        for(unsigned int col = 0; col < Map::map().cols(); col++)
            if(!grid[row][col].isWater)
                grid[row][col].reset();
}

void State::makeMoveLocal(Ant& ant)
{
	const Location& oldLoc = ant.getLocation();
	Location newLoc = ant.getNewLocation();
	grid[newLoc.row][newLoc.col].ant = grid[oldLoc.row][oldLoc.col].ant;
	grid[oldLoc.row][oldLoc.col].ant = -1;
}

void State::sendMoveToEngine(Ant& ant)
{
	const Location& loc = ant.getLocation();
	const AntDirection moveDirection = ant.getOrder().getMove();
    cout << "o " << loc.row << " " << loc.col << " " << moveDirection << endl;
	bug << "o " << loc.row << " " << loc.col << " " << moveDirection << endl;

	ant.moveByOrder();
};


bool State::getAMovingDirectionTo(const Location &locFrom, const Location &locTo, AntDirection& aDirection)
{
	bool bFoundADirection = false;
	double dMinDist = numeric_limits<double>::max();
	const vector<AntDirection> vDirections = Location::getAllDirections();
	for(auto dir : vDirections)
	{
		const Location locTestDirection = Location::getLocation(locFrom, dir);
		if(!isTargetPositionFreeToGo(locTestDirection))
		{
			continue;
		}

		const double dDist = Location::distance(locTestDirection, locTo);
		if(dDist < dMinDist)
		{
			dMinDist = dDist;
			aDirection = dir;
			bFoundADirection = true;
		}
	}
	return bFoundADirection;
}

//	returns true and a random direction with a valid target location, or false
bool State::getARandomDirectionFrom(const Location& locFrom, AntDirection& dirRandom)
{
	vector<AntDirection> vAllDirections = Location::getAllDirections();
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

bool State::isTargetPositionFreeToGo(const Location& locTo)
{
	const Square& sqTo = grid[locTo.row][locTo.col];
	if(sqTo.isWater || isAntOnPosition(locTo) || sqTo.isHill)
	{
		return false;
	}

	return true;
}

bool State::isAntOnPosition(const Location& loc)
{
	const Square& sqTo = grid[loc.row][loc.col];
	return sqTo.ant >= 0;
}

bool State::getClosestFood(const Location &locFrom, Location &locClosestFood)
{
	//	if there is non, they have to go explore
	if (food.empty())
	{
		bug << "no food found from pos" << locFrom << endl;
		locClosestFood = locFrom;
		return false;
	}

	double dMinDist = std::numeric_limits<double>::max();
	std::vector<Location>::iterator itClosestFood = food.begin();

	//	find food with shortest distance to Location
	for(std::vector<Location>::iterator it = food.begin(); it != food.end(); ++it)
	{
		double dDist = Location::distance(locFrom, *it);
		
		//	food on same position as ant or error in distance?
		if(dDist <= numeric_limits<double>::epsilon())
		{
			continue;
		}
		
		// find food with min distance
		if(dDist < dMinDist)
		{
			locClosestFood = *it;
			dMinDist = dDist;
		}
	}

	//	dont go to the food, if its too far away
	if(dMinDist > (viewradius * 2))
	{
		bug << "closest food found at pos" << locClosestFood << endl;
		return false;
	}
	else
	{
		bug << "closest food found at pos, its too far away" << locClosestFood << endl;
		return true;
	}
}

//	calculating a score for each Square in the map representing closeness to water [0...1] 0 very close, 1 far away
void State::updatePathScore()
{
	//	TODO: use spread alg to just cover areas which are visible to update that map
	
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
	Square& sq = grid[loc.row][loc.col];
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
	if(sq.isWater)
	{
		sq.pathScore = 0.0f;
		sq.pathScoreComplete = true;
		return;
	}

	//	size for 2D neighborhood of one square
	const int neighbsize = 3; // TODO: make global and able to adapt to maze, viewradius etc

	int iUnfoggedSquaresTotal = 0;
	int iWaterSquaresTotal = 0;
	int iLandSquaresTotal = 0;
	
	for(int col = -neighbsize; col < neighbsize; ++col)
	{
		for(int row = -neighbsize; row < neighbsize; ++row)
		{
			const Location locRelative = Location::getLocationRelative(loc, col, row);
			Square& sqRel = grid[locRelative.row][locRelative.col];
			if(sqRel.isFogged())
			{
				continue;
			}

			//	count water and land squares in neighborhood
			if(sqRel.isWater)
			{
				iWaterSquaresTotal++;
			}
			else if(sqRel.isLand)
			{
				iLandSquaresTotal++;
			}
			iUnfoggedSquaresTotal++;
		}
	}

	//	Score #Land to #Water - a lot of Water around makes a low Score, a lot of Land a high Score
	//	a lot of fogged squares makes a high score as well, so interesting to explore
	const int nghSize = (2 * neighbsize + 1) * (2 * neighbsize + 1);
	sq.pathScore = 1.0f - ((float)iWaterSquaresTotal / (float) nghSize);
	
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
	const vector<AntDirection> vDirections = Location::getAllDirections();

    for(const auto& ant : myAnts)
    {
		const Location& antLoc = ant.getLocation();
        std::queue<Location> locQueue;
		locQueue.push(antLoc);

        std::vector<std::vector<bool> > visited(Map::map().rows(), std::vector<bool>(Map::map().cols(), 0));
        grid[antLoc.row][antLoc.col].isVisible = 1;
        visited[antLoc.row][antLoc.col] = 1;

        while(!locQueue.empty())
        {
            const Location curLoc = locQueue.front();
            locQueue.pop();
						
			for(auto dir : vDirections)
			{
                const Location nLoc = Location::getLocation(curLoc, dir);
                if(!visited[nLoc.row][nLoc.col] && Location::distance(antLoc, nLoc) <= viewradius)
                {
					Square& nSq = grid[nLoc.row][nLoc.col];
                    nSq.isVisible = 1;
					nSq.isLand = !nSq.isWater;	//all visible squares, that aren't water are land and unfogged
                    locQueue.push(nLoc);
                }
                visited[nLoc.row][nLoc.col] = 1;
            }
        }
    }
};

/*
    This is the output function for a state. It will add a char map
    representation of the state to the output stream passed to it.

    For example, you might call "cout << state << endl;"
*/
ostream& operator<<(ostream &os, const State &state)
{
	printKnownMap(os, state);
	printScoreMap(os, state);
    return os;
};

void printKnownMap(ostream& os, const State& state)
{
	for(unsigned int row=0; row < Map::map().rows(); row++)
	{
		for(unsigned int col=0; col < Map::map().cols(); col++)
		{
			if(state.grid[row][col].isWater)
				os << '%';
			else if(state.grid[row][col].isFood)
				os << '*';
			else if(state.grid[row][col].isHill)
				os << (char)('A' + state.grid[row][col].hillPlayer);
			else if(state.grid[row][col].ant >= 0)
				os << (char)('a' + state.grid[row][col].ant);
			else if(state.grid[row][col].isVisible)
				os << '.';
			else
				os << '?';
		}
		os << endl;
	}
	os << endl;
}

void printScoreMap(ostream& os, const State& state)
{
	for(unsigned int row=0; row < Map::map().rows(); row++)
	{
		for(unsigned int col=0; col < Map::map().cols(); col++)
		{
			const float& fScore = state.grid[row][col].pathScore;
			if(fScore < 0.0f)
			{
				os << '?';
			}
			else
			{
				int score1to9 = std::min((int)(fScore * 10.0), 9);
				os << score1to9;
			}
		}
		os << endl;
	}
	os << endl;
}

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
            break;	//TODO: warum break und nicht return?
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
	int row, col, player;
	while(is >> inputType)
    {
        if(inputType == "w") //water square
        {
            is >> row >> col;
            state.grid[row][col].isWater = 1;
        }
        else if(inputType == "f") //food square
        {
            is >> row >> col;
            state.grid[row][col].isFood = 1;
            state.food.push_back(Location(row, col));
        }
        else if(inputType == "a") //live ant square
        {
            is >> row >> col >> player;
            state.grid[row][col].ant = player;
			Location antLoc(row, col);
            if(player == 0)
                state.myAnts.push_back(Ant(antLoc));
            else
                state.enemyAnts.push_back(antLoc);
        }
        else if(inputType == "d") //dead ant square
        {
            is >> row >> col >> player;
            state.grid[row][col].deadAnts.push_back(player);
        }
        else if(inputType == "h")	//hill square
        {
            is >> row >> col >> player;
            state.grid[row][col].isHill = 1;
            state.grid[row][col].hillPlayer = player;
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