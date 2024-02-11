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
    while (cin >> state)
    {
        state.updateVisionInformation();
        makeMoves();
        endTurn();
    }
}

//makes the bots moves for the turn
void Bot::makeMoves()
{
    state.bug << "turn " << state.turn << ":" << endl;
    state.bug << state << endl;

    orders.clear();
    targets.clear();

    // add all locations to unseen tiles set, run once
    if (unseenTiles.empty()) {
        for (int row = 0; row < state.rows; row++) {
            for (int col = 0; col < state.cols; col++) {
                unseenTiles.insert(Location(row, col));
            }
        }
    }
    // remove any tiles that can be seen, run each turn
    for (auto iter = unseenTiles.begin(); iter != unseenTiles.end();)
    {
        if (state.grid[iter->row][iter->col].isVisible)
        {
            iter = unseenTiles.erase(iter);
            //state.bug << "seen: " << iter->row << " " << iter->col << "\n";
        }
        else
        {
            ++iter;
        }
    }

    // prevent stepping on own hill
    for (Location myHill : state.myHills)
    {
        orders[myHill] = Location(-1, -1);
    }

    std::vector<std::tuple<double, Location, Location>> antDist;
    // find close food
    for(auto foodLoc : state.food)
    {
        for (Location antLoc : state.myAnts)
        {
            auto dist = state.distance(antLoc, foodLoc);

            antDist.emplace_back(dist, antLoc, foodLoc);
        }
    }

    std::sort(antDist.begin(), antDist.end());

    for (auto res : antDist)
    {
        Location antLoc = std::get<1>(res);
        Location foodLoc = std::get<2>(res);

        // if food has no ant gathering it
        if (targets.count(foodLoc) == 0) {
            if (!targets.empty())
            {
                // if ant has no task
                for (auto it = targets.begin(); it != targets.end(); ++it) {
                    if (!(it->second == antLoc)) {
                        makeMove(antLoc, foodLoc);
                    }
                }
            }
            else
            {
                makeMove(antLoc, foodLoc);
            }
        }
    }

	// TODO: fix perf problems for large maps (of 6p with time taken > 100ms)
    // explore unseen areas
    for (Location antLoc : state.myAnts) {
        if (!orders.count(antLoc)) {
            std::vector<std::tuple<int, Location>> unseenDist;
            for (Location unseenLoc : unseenTiles) {
                auto dist = state.distance(antLoc, unseenLoc);
                unseenDist.emplace_back(dist, unseenLoc);
            }
            std::sort(unseenDist.begin(), unseenDist.end());
            for (auto res : unseenDist) {
                Location unseenLoc = std::get<1>(res);
                if (makeMove(antLoc, unseenLoc))
                    break;
            }
        }
    }

    // unblock hills
    for (Location hillLoc : state.myHills) 
    {
        auto it = std::find(state.myAnts.cbegin(), state.myAnts.cend(), hillLoc);
        if (it != state.myAnts.end())
        {
            // an ant is on a hill
            for (int d = 0; d < TDIRECTIONS; d++)
            {
                if (makeMove(hillLoc, d)) 
                {
                    break;
                }
            }
        }
    }

    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
}

bool Bot::makeMove(const Location& loc, const Location& dest) {
    const std::vector<int> directions = state.getDirections(loc, dest);

    for (int d : directions)
    {
	    if (makeMove(loc, d))
	    {
		    targets[dest] = loc;
		    return true;
	    }
    }

    return false;
};

bool Bot::makeMove(const Location& loc, const int direction)
{
    const Location newLoc = state.getLocation(loc, direction);

    if (!state.grid[newLoc.row][newLoc.col].isWater && !state.grid[newLoc.row][newLoc.col].isMyAnt && orders.count(newLoc) == 0)
    {
        state.makeMove(loc, direction);
        orders[newLoc] = loc;
        //state.bug << "move: " << loc << " " << direction << " " << newLoc << "\n";
        return true;
    }
    return false;
};

//finishes the turn
void Bot::endTurn()
{
    if(state.turn > 0)
        state.reset();
    state.turn++;

    cout << "go" << endl;
};
