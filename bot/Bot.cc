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
    for(int ant=0; ant<(int)state.myAnts.size(); ant++)
    {
        for(int d=0; d<TDIRECTIONS; d++)
        {
            Location oldLoc = state.myAnts[ant];
            Location loc = state.getLocation(oldLoc, d);

            if(!state.grid[loc.row][loc.col].isWater && orders.count(std::make_pair(loc.row, loc.col)) == 0)
            {
                state.makeMove(oldLoc, d);
                orders[std::make_pair(loc.row, loc.col)] = std::make_pair(oldLoc.row, oldLoc.col);
                break;
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
