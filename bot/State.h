#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <stdint.h>

#include "Timer.h"
#include "Bug.h"
#include "Square.h"
#include "Location.h"

/*
    constants
*/
const int TDIRECTIONS = 4;
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}

/*
    struct to store current state information
*/
struct State
{
    /*
        Variables
    */
    int rows, cols,
        turn, turns,
        noPlayers;
    double attackRadius, spawnRadius, viewRadius;
    double loadTime, turnTime;
    std::vector<double> scores;
    bool isGameOver;
    int64_t seed;

    std::vector<std::vector<Square>> grid;
    std::vector<Location> myAnts, enemyAnts, myHills, enemyHills, food;

    Timer timer;
    Bug bug;

    /*
        Functions
    */
    State();
    ~State();

    void Setup();
    void Reset();

    double TimeRemaining();

    void MakeMove(const Location &r_loc, int direction);
    void FakeMove(const Location &r_loc, int direction);
    void UndoFakeMove(const Location &r_loc, int direction);

    double EuclideanDistance(const Location &r_loc1, const Location &r_loc2) const;
    float ManhattanDistance(Location current, Location destination) const;
    Location GetLocation(const Location &r_startLoc, int direction);
    Location GetLocation(const Location& r_startLoc, int direction, int length);
    std::vector<int> GetDirections(Location l1, Location l2);
    bool IsUnoccupied(std::map<Location, Location>& r_orders, const Location& r_newLoc) const;
    bool FakeIsUnoccupied(const Location &r_loc) const;

    void UpdateVisionInformation();
};

std::ostream& operator<<(std::ostream &r_os, const State &r_state);
std::istream& operator>>(std::istream &r_is, State &r_state);

#endif //STATE_H_
