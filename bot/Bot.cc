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

    //picks out moves for each ant
    for(Location loc : state.myAnts)
    {
        for(int d=0; d<TDIRECTIONS; d++)
        {
            if (makeMove(loc, d))
                break;
        }
    }

    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
}

bool Bot::makeMove(const Location& loc, const int direction)
{
    const Location newLoc = state.getLocation(loc, direction);

    if (!state.grid[newLoc.row][newLoc.col].isWater && orders.count(newLoc) == 0)
    {
        state.makeMove(loc, direction);
        orders[newLoc] = loc;
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
