#pragma once
#include "Pathfinding.h"
#include "Bot.h"

class AntBehavior
{

public:

	enum Behavior
	{
		explore = 0,
		attack_ants = 1,
		gather_food = 2,
	};

	Behavior _currentBehavior;
	Bot* _bot;

	AntBehavior(Bot* bot) : _currentBehavior(explore) {};
	virtual void MakeMoves();
	void MoveToLocation(Location startAnt, Location destinationAnt);
	void ExploreMap();
	void UnblockHills();

protected:

	Pathfinding* _pathfinding;
};

