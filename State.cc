#include "State.h"

using namespace std;

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
    grid = vector<vector<Square> >(rows, vector<Square>(cols, Square()));
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
	for(int row=0; row < rows; row++)
        for(int col=0; col<cols; col++)
            if(!grid[row][col].isWater)
                grid[row][col].reset();
}

//outputs move information to the engine
void State::makeMove(const Location &oldLoc, int moveDirection)
{
    cout << "o " << oldLoc.row << " " << oldLoc.col << " " << CDIRECTIONS[moveDirection] << endl;
	bug << "o " << oldLoc.row << " " << oldLoc.col << " " << CDIRECTIONS[moveDirection] << endl;

    Location newLoc = getLocation(oldLoc, moveDirection);
    grid[newLoc.row][newLoc.col].ant = grid[oldLoc.row][oldLoc.col].ant;
    grid[oldLoc.row][oldLoc.col].ant = -1;
};

//returns the euclidean distance between two locations with the edges wrapped
double State::distance(const Location &loc1, const Location &loc2)
{
    int d1 = abs(loc1.row-loc2.row),
        d2 = abs(loc1.col-loc2.col),
        dr = min(d1, rows-d1),
        dc = min(d2, cols-d2);
    return sqrt(dr*dr + dc*dc);
};

//returns the new location from moving in a given direction with the edges wrapped
Location State::getLocation(const Location &loc, int direction)
{
    return Location( (loc.row + DIRECTIONS[direction][0] + rows) % rows,
                     (loc.col + DIRECTIONS[direction][1] + cols) % cols );
};

//TODO: return some type of TDIRECTIONS Type;
//
bool State::getAMovingDirectionTo(const Location &locFrom, const Location &locTo, int& aDirection)
{
	bool bFoundADirection = false;
	double dMinDist = numeric_limits<double>::max();
	for(int dir = 0; dir < TDIRECTIONS; ++dir)
	{
		const Location locTestDirection = getLocation(locFrom, dir);
		if(!isTargetPositionFreeToGo(locTestDirection))
		{
			continue;
		}

		const double dDist = distance(locTestDirection, locTo);
		if(dDist < dMinDist)
		{
			dMinDist = dDist;
			aDirection = dir;
			bFoundADirection = true;
		}
	}
	return bFoundADirection;
}

bool State::isTargetPositionFreeToGo(const Location& locTo)
{
	const Square& sqTo = grid[locTo.row][locTo.col];
	if(sqTo.isWater || isAntOnPosition(locTo))
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
		double dDist = distance(locFrom, *it);
		
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
    for(int a=0; a<(int) myAnts.size(); a++)
    {
        const Location antLoc = myAnts[a];
        std::queue<Location> locQueue;
		locQueue.push(antLoc);

        std::vector<std::vector<bool> > visited(rows, std::vector<bool>(cols, 0));
        grid[antLoc.row][antLoc.col].isVisible = 1;
        visited[antLoc.row][antLoc.col] = 1;

        while(!locQueue.empty())
        {
            const Location curLoc = locQueue.front();
            locQueue.pop();

            for(int d=0; d<TDIRECTIONS; d++)
            {
                const Location nLoc = getLocation(curLoc, d);

                if(!visited[nLoc.row][nLoc.col] && distance(antLoc, nLoc) <= viewradius)
                {
                    grid[nLoc.row][nLoc.col].isVisible = 1;
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
    for(int row=0; row<state.rows; row++)
    {
        for(int col=0; col<state.cols; col++)
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

    while(is >> inputType)
    {
        if(inputType == "loadtime")
            is >> state.loadtime;
        else if(inputType == "turntime")
            is >> state.turntime;
        else if(inputType == "rows")
            is >> state.rows;
        else if(inputType == "cols")
            is >> state.cols;
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
            if(player == 0)
                state.myAnts.push_back(Location(row, col));
            else
                state.enemyAnts.push_back(Location(row, col));
        }
        else if(inputType == "d") //dead ant square
        {
            is >> row >> col >> player;
            state.grid[row][col].deadAnts.push_back(player);
        }
        else if(inputType == "h")
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
            is >> state.noPlayers;
        else if(inputType == "scores") //score information
        {
            state.scores = vector<double>(state.noPlayers, 0.0);
            for(int p=0; p<state.noPlayers; p++)
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