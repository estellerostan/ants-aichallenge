#include "State.h"

#include <map>

using namespace std;

//constructor
State::State()
{
    isGameOver = false;
    turn = 0;
    bug.Open("../../debug.log");
};

//deconstructor
State::~State()
{
    bug.Close();
};

//sets the state up
void State::Setup()
{
    grid = vector<vector<Square> >(rows, vector<Square>(cols, Square()));
};

//resets all non-water squares to land and clears the bots ant vector
void State::Reset()
{
    myAnts.clear();
    enemyAnts.clear();
    myHills.clear();
    enemyHills.clear();
    food.clear();
    for(int row=0; row<rows; row++)
        for(int col=0; col<cols; col++)
            if(!grid[row][col].isWater)
                grid[row][col].Reset();
};

// Calculates the time left before a timeout, useful to check to abort slow code
double State::TimeRemaining() {
    return turnTime - timer.GetTime();
}

//outputs move information to the engine
void State::MakeMove(const Location &r_loc, int direction)
{
    cout << "o " << r_loc.row << " " << r_loc.col << " " << CDIRECTIONS[direction] << endl;

    Location nLoc = GetLocation(r_loc, direction);
    grid[nLoc.row][nLoc.col].ant = grid[r_loc.row][r_loc.col].ant;
    grid[r_loc.row][r_loc.col].ant = -1;
};

//DOES NOT output move information to the engine
void State::FakeMove(const Location& r_loc, int direction)
{
    const Location nLoc = GetLocation(r_loc, direction);
    grid[nLoc.row][nLoc.col].ant = grid[r_loc.row][r_loc.col].ant;
    grid[r_loc.row][r_loc.col].ant = -1;
};

// always use this after FakeMove to not mess up the state
void State::UndoFakeMove(const Location& r_loc, int direction)
{
    const Location nLoc = GetLocation(r_loc, direction);
    grid[r_loc.row][r_loc.col].ant = grid[nLoc.row][nLoc.col].ant;
    grid[nLoc.row][nLoc.col].ant = -1;
};

//returns the euclidean distance between two locations with the edges wrapped
double State::EuclideanDistance(const Location &r_loc1, const Location &r_loc2) const
{
    int d1 = abs(r_loc1.row-r_loc2.row),
        d2 = abs(r_loc1.col-r_loc2.col),
        dr = min(d1, rows-d1),
        dc = min(d2, cols-d2);
    return sqrt(dr*dr + dc*dc);
};

//returns the Manhattan distance between two locations with the edges wrapped
float State::ManhattanDistance(Location current, Location destination) const {
    const int dx = abs(current.row - destination.row),
        dy = abs(current.col - destination.col),
        dr = min(dx, rows - dx),
        dc = min(dy, cols - dy);
    return static_cast<float>(dr + dc);
}

//returns the new location from moving in a given direction with the edges wrapped
Location State::GetLocation(const Location &loc, int direction) const
{
    return Location( (loc.row + DIRECTIONS[direction][0] + rows) % rows,
                     (loc.col + DIRECTIONS[direction][1] + cols) % cols );
};

//returns the new location from moving in a given direction for a specified length with the edges wrapped
Location State::GetLocation(const Location& loc, int direction, int length) const
{
    return Location((loc.row + (DIRECTIONS[direction][0] * length) + rows) % rows,
        (loc.col + (DIRECTIONS[direction][1] * length) + cols) % cols);
};

std::vector<int> State::GetDirections(Location l1, Location l2) const {
    std::vector<int> directions = std::vector<int>();

    if (l1.row < l2.row)
    {
        if (l2.row - l1.row >= rows / 2)
        {
            directions.push_back(0);
        }
        else
        {
            directions.push_back(2);
        }
    }
    else if (l1.row > l2.row)
    {
        if (l1.row - l2.row >= rows / 2)
        {
            directions.push_back(2);
        }
        else
        {
            directions.push_back(0);
        }
    }

    if (l1.col < l2.col)
    {
        if (l2.col - l1.col >= cols / 2)
        {
            directions.push_back(3);
        }
        else {
            directions.push_back(1);
        }
    }
    else if (l1.col > l2.col)
    {
        if (l1.col - l2.col >= cols / 2)
        {
            directions.push_back(1);
        }
        else
        {
            directions.push_back(3);
        }
    }

    return directions;
}

std::vector<int> State::GetOppositeDirections(Location l1, Location l2) const {
    std::vector<int> directions = std::vector<int>();

    if (l1.row < l2.row)
    {
        if (l2.row - l1.row >= rows / 2)
        {
            directions.push_back(2);
        }
        else
        {
            directions.push_back(0);
        }
    }
    else if (l1.row > l2.row)
    {
        if (l1.row - l2.row >= rows / 2)
        {
            directions.push_back(0);
        }
        else
        {
            directions.push_back(2);
        }
    }

    if (l1.col < l2.col)
    {
        if (l2.col - l1.col >= cols / 2)
        {
            directions.push_back(1);
        }
        else {
            directions.push_back(3);
        }
    }
    else if (l1.col > l2.col)
    {
        if (l1.col - l2.col >= cols / 2)
        {
            directions.push_back(3);
        }
        else
        {
            directions.push_back(1);
        }
    }

    return directions;
}

/**
 * \brief Check if the NEW location (not current) an ant wants to go to is available or not.
 *        This allows ants to follow each other, depending on the (insertion) order their next moves are added in Bot::_orders.
 * \param r_orders The map keeping track of every NEW locations, updated each time an ant commits to a move.
 * \param r_newLoc The new location we want to check for availability
 * \return 
 */
bool State::IsUnoccupied(map<Location, Location>& r_orders, const Location& r_newLoc) const
{
    if (grid[r_newLoc.row][r_newLoc.col].isWater)
    {
        return false;
    }

    bool canMove = false;
    Location location{ -1, -1 };
    for (auto it = r_orders.begin(); it != r_orders.end(); ++it)
    {
        if (it->second == r_newLoc)
        {
            canMove = true;
            location = it->first;
            break;
        }
    }

    // Check first if the location we want to go to will be available or not (current ant didn't move).
	// This check allows to follow another ant if newLoc is set.
    if ((!canMove && grid[r_newLoc.row][r_newLoc.col].isMyAnt) || (canMove && location == Location{ -1, -1 }) || r_orders.count(r_newLoc) != 0)
    {
        return false;
    }
    return true;
}

bool State::FakeIsUnoccupied(const Location& r_loc) const
{
    // IsUnoccupied can't be used because isMyAnt waits for the turn to be played
    // Note: grid[r_loc.row][r_loc.col].ant == 0 is own ant, > 1 are enemies.
    return !grid[r_loc.row][r_loc.col].isWater && grid[r_loc.row][r_loc.col].ant != 0;
}

/*
    This function will update update the lastSeen value for any squares currently
    visible by one of your live ants.

    BE VERY CAREFUL IF YOU ARE GOING TO TRY AND MAKE THIS FUNCTION MORE EFFICIENT,
    THE OBVIOUS WAY OF TRYING TO IMPROVE IT BREAKS USING THE EUCLIDEAN METRIC, FOR
    A CORRECT MORE EFFICIENT IMPLEMENTATION, TAKE A LOOK AT THE GET_VISION FUNCTION
    IN ANTS.PY ON THE CONTESTS GITHUB PAGE.
*/
void State::UpdateVisionInformation()
{
    std::queue<Location> locQueue;
    Location sLoc, cLoc, nLoc;

    for(int a=0; a<(int) myAnts.size(); a++)
    {
        sLoc = myAnts[a];
        locQueue.push(sLoc);

        std::vector<std::vector<bool> > visited(rows, std::vector<bool>(cols, 0));
        grid[sLoc.row][sLoc.col].isVisible = true;
        visited[sLoc.row][sLoc.col] = true;

        while(!locQueue.empty())
        {
            cLoc = locQueue.front();
            locQueue.pop();

            for(int d=0; d<TDIRECTIONS; d++)
            {
                nLoc = GetLocation(cLoc, d);

                if(!visited[nLoc.row][nLoc.col] && EuclideanDistance(sLoc, nLoc) <= viewRadius)
                {
                    grid[nLoc.row][nLoc.col].isVisible = true;
                    locQueue.push(nLoc);
                }
                visited[nLoc.row][nLoc.col] = true;
            }
        }
    }
};

/*
    This is the output function for a state. It will add a char map
    representation of the state to the output stream passed to it.

    For example, you might call "cout << state << endl;"
*/
ostream& operator<<(ostream &r_os, const State &r_state)
{
    for(int row=0; row<r_state.rows; row++)
    {
        for(int col=0; col<r_state.cols; col++)
        {
            if(r_state.grid[row][col].isWater)
                r_os << '%';
            else if(r_state.grid[row][col].isFood)
                r_os << '*';
            else if(r_state.grid[row][col].isHill)
                r_os << (char)('A' + r_state.grid[row][col].hillPlayer);
            else if(r_state.grid[row][col].ant >= 0)
                r_os << (char)('a' + r_state.grid[row][col].ant);
            else if(r_state.grid[row][col].isVisible)
                r_os << '.';
            else
                r_os << '?';
        }
        r_os << endl;
    }

    return r_os;
};

//input function
istream& operator>>(istream &r_is, State &r_state)
{
    int row, col, player;
    string inputType, junk;

    //finds out which turn it is
    while(r_is >> inputType)
    {
        if(inputType == "end")
        {
            r_state.isGameOver = true;
            break;
        }
        else if(inputType == "turn")
        {
            r_is >> r_state.turn;
            break;
        }
        else //unknown line
            getline(r_is, junk);
    }

    if(r_state.turn == 0)
    {
        //reads game parameters
        while(r_is >> inputType)
        {
            if(inputType == "loadtime")
                r_is >> r_state.loadTime;
            else if(inputType == "turntime")
                r_is >> r_state.turnTime;
            else if(inputType == "rows")
                r_is >> r_state.rows;
            else if(inputType == "cols")
                r_is >> r_state.cols;
            else if(inputType == "turns")
                r_is >> r_state.turns;
            else if(inputType == "player_seed")
                r_is >> r_state.seed;
            else if(inputType == "viewradius2")
            {
                r_is >> r_state.viewRadius;
                r_state.viewRadius = sqrt(r_state.viewRadius);
            }
            else if(inputType == "attackradius2")
            {
                r_is >> r_state.attackRadius;
                r_state.attackRadius = sqrt(r_state.attackRadius);
            }
            else if(inputType == "spawnradius2")
            {
                r_is >> r_state.spawnRadius;
                r_state.spawnRadius = sqrt(r_state.spawnRadius);
            }
            else if(inputType == "ready") //end of parameter input
            {
                r_state.timer.Start();
                break;
            }
            else    //unknown line
                getline(r_is, junk);
        }
    }
    else
    {
        //reads information about the current turn
        while(r_is >> inputType)
        {
            if(inputType == "w") //water square
            {
                r_is >> row >> col;
                r_state.grid[row][col].isWater = true;
            }
            else if(inputType == "f") //food square
            {
                r_is >> row >> col;
                r_state.grid[row][col].isFood = true;
                r_state.food.emplace_back(row, col);
            }
            else if(inputType == "a") //live ant square
            {
                r_is >> row >> col >> player;
                r_state.grid[row][col].ant = player;
                if (player == 0)
                {
                    r_state.grid[row][col].isMyAnt = true;
                    r_state.myAnts.emplace_back(row, col);
                }
                else 
                {
                    r_state.grid[row][col].isEnemyAnt = true;
                    r_state.enemyAnts.emplace_back(row, col);
                }
            }
            else if(inputType == "d") //dead ant square
            {
                r_is >> row >> col >> player;
                r_state.grid[row][col].deadAnts.push_back(player);
            }
            else if(inputType == "h")
            {
                r_is >> row >> col >> player;
                r_state.grid[row][col].isHill = true;
                r_state.grid[row][col].hillPlayer = player;
                if(player == 0)
                    r_state.myHills.emplace_back(row, col);
                else
                    r_state.enemyHills.emplace_back(row, col);

            }
            else if(inputType == "players") //player information
                r_is >> r_state.noPlayers;
            else if(inputType == "scores") //score information
            {
                r_state.scores = vector<double>(r_state.noPlayers, 0.0);
                for(int p=0; p<r_state.noPlayers; p++)
                    r_is >> r_state.scores[p];
            }
            else if(inputType == "go") //end of turn input
            {
                if(r_state.isGameOver)
                    r_is.setstate(std::ios::failbit);
                else
                    r_state.timer.Start();
                break;
            }
            else //unknown line
                getline(r_is, junk);
        }
    }

    return r_is;
};
