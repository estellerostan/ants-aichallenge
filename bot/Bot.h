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
	void PlayGame();    //plays a single game of Ants

private:
	std::map<Location, Location> _orders;
	std::map<Location, Location> _targets;
	std::set<Location> _unseenTiles;
	std::set<Location> _enemyHills;
	std::vector<std::tuple<double, Location, Location>> _attackGroups;

	State _state;
	MiniMax _miniMax;
	AStar _aStar;

	void MakeMoves();   //makes moves for a single turn
	static bool ContainsValue(std::map<Location, Location>& r_locMap, const Location& r_antLoc);
	void Setup();
	void GatherFood();
	void UnblockHills();
	void ExploreMap();
	void AttackHills();
	void CreateAttackGroups();
	void AttackAnts();
	void DefendHills();
	bool MakeMove(const Location& r_loc, const Location& r_dest, const std::string& r_from = {}); // makes a move following a destination for a single ant
	bool MakeMove(const Location& r_loc, int direction, const std::string& r_from = {}); // makes a move to a direction for a single ant
	void EndTurn();     //indicates to the engine that it has made its moves
};

#endif //BOT_H_
