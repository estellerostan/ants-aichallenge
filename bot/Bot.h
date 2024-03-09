#ifndef BOT_H_
#define BOT_H_

#include "State.h"
#include <map>
#include <set>

#include "Ant.h"

/*
	This struct represents your bot in the game of Ants
*/
struct Bot
{

	std::map<Location, Location> orders;
	std::map<Location, Location> targets;
	std::set<Location> unseenTiles;
	std::set<Location> enemyHills;


	// TODO: move declarations in their own class
	std::vector<Ant*> myAnts;
	std::vector<Ant*> enemyAnts;
	int range = 99999;
	int bestValue = -range;

	State state;

	Bot();

	void playGame();    //plays a single game of Ants

	void makeMoves();   //makes moves for a single turn
	static bool containsValue(std::map<Location, Location>& locMap, const Location& antLoc);
	void setup();
	void gatherFood();
	void unblockHills();
	void exploreMap();
	void attackHills();
	// TODO: move functions in their own class
	void attackAnts();
	int miniMaxMin(int antIndex);
	void miniMaxMax(int antIndex);
	int minMaxEvaluate();
	bool makeMove(const Location& loc, const Location& dest); // makes a move following a destination for a single ant
	bool makeMove(const Location& loc, int direction); // makes a move to a direction for a single ant
	void endTurn();     //indicates to the engine that it has made its moves
};

#endif //BOT_H_
