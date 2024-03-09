#include "Bot.h"
#include "AStar.h"
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
	while (cin >> state)
	{
		state.updateVisionInformation();
		makeMoves();
		endTurn();
	}
}

//makes the bots moves for the turn
void Bot::makeMoves()
{
	state.bug << "turn " << state.turn << ":" << endl;
	state.bug << state << endl;

    setup();
    gatherFood();
    attackHills();
    attackAnts();
	// TODO: fix perf problems for large maps (of 6p with time taken > 100ms)
	// explore unseen areas
	exploreMap();
	unblockHills();

	// TODO: Remove this test.
	const Location start{ 0, 28 }, goal{ 27, 10 };
	std::map<Location, Location> cameFrom;
	std::map<Location, double> costSoFar;
	const auto aStar = AStar(&state);

	aStar.AStarSearch(start, goal, cameFrom, costSoFar);
	const std::vector<Location> res = aStar.ReconstructPath(start, goal, cameFrom);
	state.bug << "AStar: " << res.size() << endl;
	if (!res.empty()) {
		state.bug << "next move: " << res.front() << endl; // Only if goal =/= start and goal not water.
	}
	for (auto from : res)
	{
		state.bug << from << endl;
	}

	// TODO: Remove this test.
	const Location startBFS{ 30, 19 }, goalBFS{ 31, 10 };
	const auto resBFS = aStar.BreadthFirstSearch(startBFS, goalBFS);
	state.bug << "BFS: " << resBFS.size() << endl;
	std::vector<Location> foodLocs;
	for (std::pair<Location, Location> from : resBFS)
	{
		if (state.grid[from.first.row][from.first.col].isFood)
		{
			foodLocs.push_back(from.first);
			state.bug << from.first << endl;
		}
	}
	state.bug << "food amount:" << foodLocs.size() << endl;

	// TODO: Remove this test.
	const Location startBFS2{ 9, 16 }, goalBFS2{ 15, 16 };
	const auto resBFS2 = aStar.BreadthFirstSearch(startBFS2, goalBFS2, ENEMYANT, 5);
	state.bug << "BFS2: " << resBFS2.size() << endl;
	for (std::pair<Location, Location> from : resBFS2)
	{
		state.bug << "ant nearest to food " << from.first << endl;
	}

	state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
}

void Bot::setup()
{
    orders.clear();
    targets.clear();
    myAnts.clear();
    enemyAnts.clear();

	// add all locations to unseen tiles set, run once
	// necessary for Bot::exploreMap
	if (unseenTiles.empty()) {
		for (int row = 0; row < state.rows; row++) {
			for (int col = 0; col < state.cols; col++) {
				unseenTiles.insert(Location(row, col));
			}
		}
	}
	// remove any tiles that can be seen, run each turn
	for (auto iter = unseenTiles.begin(); iter != unseenTiles.end();)
	{
		if (state.grid[iter->row][iter->col].isVisible)
		{
			iter = unseenTiles.erase(iter);
			//state.bug << "seen: " << iter->row << " " << iter->col << "\n";
		}
		else
		{
			++iter;
		}
	}

	// prevent stepping on own hill
	// necessary for Bot::unblockHills
	for (Location myHill : state.myHills)
	{
		orders[myHill] = Location(-1, -1);
	}
}

void Bot::gatherFood()
{
	std::vector<std::tuple<double, Location, Location>> antDist;
	// find close food
	for (auto foodLoc : state.food)
	{
		for (Location antLoc : state.myAnts)
		{
			auto dist = state.EuclideanDistance(antLoc, foodLoc);

			antDist.emplace_back(dist, antLoc, foodLoc);
		}
	}

	std::sort(antDist.begin(), antDist.end());

	for (auto res : antDist)
	{
		Location antLoc = std::get<1>(res);
		Location foodLoc = std::get<2>(res);

		// if food has no ant gathering it and ant has no task
		const bool isAntBusyWithFood = containsValue(targets, antLoc);
		if (targets.count(foodLoc) == 0 && !isAntBusyWithFood) {
			makeMove(antLoc, foodLoc);
		}
	}
}

void Bot::unblockHills()
{
	for (Location hillLoc : state.myHills)
	{
		auto it = std::find(state.myAnts.cbegin(), state.myAnts.cend(), hillLoc);
		const bool hasMove = containsValue(orders, hillLoc);
		if (it != state.myAnts.end() && !hasMove)
		{
			// an ant is on a hill
			for (int d = 0; d < TDIRECTIONS; d++)
			{
				if (makeMove(hillLoc, d))
				{
					break;
				}
			}
		}
	}
}

void Bot::exploreMap()
{
	for (Location antLoc : state.myAnts)
	{
		const bool hasMove = containsValue(orders, antLoc);
		if (!hasMove)
		{
			std::vector<std::tuple<int, Location>> unseenDist;
			for (Location unseenLoc : unseenTiles)
			{
				auto dist = state.EuclideanDistance(antLoc, unseenLoc);
				unseenDist.emplace_back(dist, unseenLoc);
				// avoid timeout, even if the search is not complete
				// better than being removed from the game
				if (state.timeRemaining() < 200) {
					state.bug << "explore " << endl;
					break;
				}
			}
			std::sort(unseenDist.begin(), unseenDist.end());
			for (auto res : unseenDist)
			{
				Location unseenLoc = std::get<1>(res);
				if (makeMove(antLoc, unseenLoc)) {
					break;
				}
			}
		}
	}
}

void Bot::attackHills()
{
	for (auto enemyHill : state.enemyHills)
	{
		if (enemyHills.count(enemyHill) == 0)
		{
			enemyHills.insert(enemyHill);
		}
	}

	std::vector<std::tuple<double, Location, Location>> antDistToHill;
	for (auto hillLoc : enemyHills)
	{
		for (Location antLoc : state.myAnts)
		{
			const bool hasMove = containsValue(orders, antLoc);
			if (!hasMove) {
				auto dist = state.EuclideanDistance(antLoc, hillLoc);
				antDistToHill.emplace_back(dist, antLoc, hillLoc);
			}
		}
	}
	std::sort(antDistToHill.begin(), antDistToHill.end());
	for (auto res : antDistToHill)
	{
		Location antLoc = std::get<1>(res);
		Location hillLoc = std::get<2>(res);
		makeMove(antLoc, hillLoc);
	}
}

void Bot::attackAnts()
{
    // TODO: finish minimix: group ants by attacks
    //for (Location ant : state.myAnts)
    //{
    //    if (ant.row == 28) continue;

    //    const bool hasMove = containsValue(orders, ant);
    //    if (hasMove) continue;
    //    myAnts.push_back(new Ant(ant));
    //}

    //for (Location ant : state.enemyAnts)
    //{
    //    if (ant.row == 14) continue;
    //    enemyAnts.push_back(new Ant(ant));
    //}

    // minimax algorithm adapted from http://www.xathis.com//posts/ai-challenge-2011-ants.html
    bestValue = -range;
    // TODO: optimize minimax for large groups of ants and lots of attacks at once.
    miniMaxMax(0);
    for (const Ant* myAnt : myAnts) {
        if (myAnt->bestDest != Location(-1, -1)) makeMove(myAnt->loc, myAnt->bestDest); // else?
    }
}

// Always start with maximizing player = own ants
void Bot::miniMaxMax(int antIndex)
{
    if (state.timeRemaining() < 200) {
        state.bug << "minimax timeout" << endl;
        return;
    }

    if (antIndex < static_cast<int>(myAnts.size()))
    {
        Ant* myAnt = myAnts[antIndex];
        // TODO: optimize/ skip directions
        for (int d = 0; d < TDIRECTIONS; d++)
        {
            const Location newLoc = state.getLocation(myAnt->loc, d);
            if (state.fakeIsUnoccupied(newLoc)) {
                state.fakeMove(myAnt->loc, d);
                myAnt->dest = newLoc;
                miniMaxMax(antIndex + 1);
                state.undoFakeMove(myAnt->loc, d);
            }
        }
    }
    else
    {
        const int value = miniMaxMin(0);
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

int Bot::miniMaxMin(int antIndex)
{
    if (antIndex < static_cast<int>(enemyAnts.size()))
    {
        int minBestValue = +range;
        Ant* enemyAnt = enemyAnts[antIndex];
        // TODO: optimize/ skip directions
        for (int d = 0; d < TDIRECTIONS; d++)
        {
            const Location newLoc = state.getLocation(enemyAnt->loc, d);
            if (state.fakeIsUnoccupied(newLoc)) {
                state.fakeMove(enemyAnt->loc, d);
                enemyAnt->dest = newLoc;
                const int value = miniMaxMin(antIndex + 1);
                state.undoFakeMove(enemyAnt->loc, d);
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

    return minMaxEvaluate();
}

int Bot::minMaxEvaluate()
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
            const auto dist = state.distance(ant->dest, enemyAnt->dest);
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
                const auto dist = state.distance(ant->dest, enemyAnt->dest);
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

bool Bot::containsValue(std::map<Location, Location>& locMap, const Location& antLoc)
{
	if (locMap.empty())
	{
		return false;
	}

	for (auto it = locMap.begin(); it != locMap.end(); ++it)
	{
		if (it->second == antLoc)
		{
			return true;
		}
	}
	return false;
}

bool Bot::makeMove(const Location& loc, const Location& dest) {
	const std::vector<int> directions = state.getDirections(loc, dest);

	for (const int d : directions)
	{
		if (makeMove(loc, d))
		{
			targets[dest] = loc;
			return true;
		}
	}

	return false;
};

bool Bot::makeMove(const Location& loc, const int direction)
{
	const Location newLoc = state.getLocation(loc, direction);

	const bool canMakeMove = state.isUnoccupied(newLoc) && orders.count(newLoc) == 0;
	if (!canMakeMove)
	{
		return false;
	}

	state.makeMove(loc, direction);
	orders[newLoc] = loc;
	//state.bug << "move: " << loc << " " << direction << " " << newLoc << "\n";
	return true;
};

//finishes the turn
void Bot::endTurn()
{
	if (state.turn > 0)
		state.reset();
	state.turn++;

	cout << "go" << endl;
};
