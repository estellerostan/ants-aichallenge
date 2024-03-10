#include "MiniMax.h"

#include <ostream>

#include "State.h"


MiniMax::MiniMax() = default;

MiniMax::MiniMax(State* state) : _state(state)
{
}

// Always start with maximizing player = own ants
void MiniMax::Max(int antIndex)
{
    if (_state->timeRemaining() < 200) {
        _state->bug << "minimax (max) timeout" << std::endl;
        return;
    }

    if (antIndex < static_cast<int>(myAnts.size()))
    {
        Ant* myAnt = myAnts[antIndex];
        // TODO: optimize/ skip directions
        for (int d = 0; d < TDIRECTIONS; d++)
        {
            const Location newLoc = _state->getLocation(myAnt->loc, d);
            if (_state->fakeIsUnoccupied(newLoc)) {
                _state->fakeMove(myAnt->loc, d);
                myAnt->dest = newLoc;
                Max(antIndex + 1);
                _state->undoFakeMove(myAnt->loc, d);
            }
        }
    }
    else
    {
        const int value = Min(0);
        if (value > bestValue)
        {
            bestValue = value;
            // save the current best moves of own ants
            for (Ant* ant : myAnts) {
                ant->bestDest = ant->dest;
            }
        }
    }
}

int MiniMax::Min(int antIndex)
{
    if (_state->timeRemaining() < 200) {
        _state->bug << "minimax (min) timeout" << std::endl;
        return bestValue;
    }

    if (antIndex < static_cast<int>(enemyAnts.size()))
    {
        int minBestValue = +range;
        Ant* enemyAnt = enemyAnts[antIndex];
        // TODO: optimize/ skip directions
        for (int d = 0; d < TDIRECTIONS; d++)
        {
            const Location newLoc = _state->getLocation(enemyAnt->loc, d);
            if (_state->fakeIsUnoccupied(newLoc)) {
                _state->fakeMove(enemyAnt->loc, d);
                enemyAnt->dest = newLoc;
                const int value = Min(antIndex + 1);
                _state->undoFakeMove(enemyAnt->loc, d);
                if (value < bestValue)
                {
                    return -range;
                }
                if (value < minBestValue)
                {
                    minBestValue = value;
                }
            }
        }
        return minBestValue;
    }

    return Evaluate();
}

int MiniMax::Evaluate() const
{
    int nbDeadEnemies = 0;
    int nbDeadAnts = 0;
    for (Ant* enemyAnt : enemyAnts)
    {
        enemyAnt->isDead = false;
        enemyAnt->nbAttackers = 0;
        for (Ant* ant : myAnts)
        {
            if (ant->dest == Location(-1, -1)) return -range;
            const auto dist = _state->EuclideanDistance(ant->dest, enemyAnt->dest);
            if (dist <= 2) // TODO: change distance to kill?
            {
                ant->nbAttackers++;
                enemyAnt->nbAttackers++;
            }
        }
    }

    for (Ant* ant : myAnts)
    {
        if (ant->nbAttackers != 0) {
            for (Ant* enemyAnt : enemyAnts)
            {
                const auto dist = _state->EuclideanDistance(ant->dest, enemyAnt->dest);
                if (enemyAnt->nbAttackers == 0 || dist > 2) {
                    // not in a fight
                    continue;
                }
                if (!enemyAnt->isDead && enemyAnt->nbAttackers >= ant->nbAttackers) {
                    nbDeadEnemies++;
                    enemyAnt->isDead = true;
                }
                if (!ant->isDead && ant->nbAttackers >= enemyAnt->nbAttackers) {
                    nbDeadAnts++;
                    ant->isDead = true;
                }
            }
            ant->isDead = false;
            ant->nbAttackers = 0;
        }
    }

    // check for number of dead enemy ants minus own ants to see who wins this fight
    // TODO: avoid 1v1? where nbDeadEnemies - nbDeadAnts is 1 - 1 == 0 (what about 2v2, 3v3, ...?)
    return nbDeadEnemies - nbDeadAnts;
}
