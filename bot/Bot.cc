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
        state.updateVisionInformation();
        makeMoves();
        endTurn();
    }
};

//makes the bots moves for the turn
void Bot::makeMoves()
{
    state.bug << "turn " << state.turn << ":" << endl;
    state.bug << state << endl;

    orders.clear();
    targets.clear();

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

    if (!state.grid[newLoc.row][newLoc.col].isWater && orders.count(newLoc) == 0)
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
