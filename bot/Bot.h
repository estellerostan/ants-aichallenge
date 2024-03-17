#ifndef BOT_H_
#define BOT_H_

#include "State.h"
#include <map>
#include <set>

#include "AStar.h"
#include "AStarV2.h"
#include "Pathfinding.h"

#include "AntBehavior.h"
#include "FoodHunt.h"
#include "Attack.h"


/*
	This struct represents your bot in the game of Ants
*/
struct Bot
{
	enum Behavior
	{
		explore = 0,
		attack_ants = 1,
		gather_food = 2,
	};

	std::map<Location, Location> orders;
	std::map<Location, Location> targets;
	std::set<Location> unseenTiles;
	std::set<Location> enemyHills;
	std::set<Location> myHills;

	State state;

	Behavior currentBehavior = gather_food;
	//Ordered by priority
	std::map<Behavior, AntBehavior*> behaviorTree;

	Bot();

	void playGame();    //plays a single game of Ants

	void makeMoves();   //makes moves for a single turn
	static bool containsValue(std::map<Location, Location>& locMap, const Location& antLoc);
	void setup();
	void gatherFood();
	void unblockHills();
	void exploreMap();
	void attackHills();
	bool makeMove(const Location& loc, const Location& dest); // makes a move following a destination for a single ant
	bool makeMove(const Location& loc, int direction); // makes a move to a direction for a single ant
	void endTurn();     //indicates to the engine that it has made its moves
};

#endif //BOT_H_
