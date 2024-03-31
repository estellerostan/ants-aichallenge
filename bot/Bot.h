#ifndef BOT_H_
#define BOT_H_

#include "State.h"
#include "AStar.h"

#include <map>
#include <set>

#include "Ant.h"
#include "MiniMax.h"

/*
	This struct represents your bot in the game of Ants
*/
struct Bot
{
	Bot();
	void playGame();    //plays a single game of Ants

private:
	std::map<Location, Location> orders;
	std::map<Location, Location> targets;
	std::set<Location> unseenTiles;
	std::set<Location> enemyHills;
	std::vector<std::tuple<double, Location, Location>> _attackGroups;

	State state;
	MiniMax miniMax;
	AStar aStar;	

	void makeMoves();   //makes moves for a single turn
	static bool containsValue(std::map<Location, Location>& locMap, const Location& antLoc);
	void setup();
	void gatherFood();
	void unblockHills();
	void exploreMap();
	void attackHills();
	void CreateAttackGroups();
	void attackAnts();
	bool makeMove(const Location& loc, const Location& dest, const std::string& from = {}); // makes a move following a destination for a single ant
	bool makeMove(const Location& loc, int direction, const std::string& from = {}); // makes a move to a direction for a single ant
	void endTurn();     //indicates to the engine that it has made its moves
};

#endif //BOT_H_
