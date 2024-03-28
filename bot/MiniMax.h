/*
 * Adapted from https://web.archive.org/web/20140413003235/http://www.xathis.com//posts/ai-challenge-2011-ants.html
 */

#pragma once
#include <vector>
#include <ostream> // used for file debug

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

	/**
	 * \brief Try to get the highest number of kills possible for own group of ants (max 5 ants),
	 * while assuming the enemy will also try to get the highest number of kills possible.
	 * The arrays Minimax::myAnts and Minimax::enemyAnts have to be filled to get a result.
	 *
	 * Warning: This code has no optimizations and will often end before simulating all possible moves.
	 * This means it sometimes cannot find the best moves and that it takes a long time to run (max 800 ms).
	 * \param antIndex Call Minimax::max(0) to get the saved best moves in MiniMax::myAnts and retrieve their bestDest.
	 */
	void Max(int antIndex);
};

