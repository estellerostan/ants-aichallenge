#pragma once
#include <vector>
#include <ostream>

#include "Ant.h"
#include "State.h"

class MiniMax
{
	State* _state;

	int Min(int antIndex);
	int Evaluate() const;

public:
	MiniMax();
	MiniMax(State* state);

	std::vector<Ant*> myAnts;
	std::vector<Ant*> enemyAnts;
	int range = 99999;
	int bestValue = -range;

	void Max(int antIndex);
};

