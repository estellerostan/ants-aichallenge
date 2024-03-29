#include "Bot.h"
#include "AStar.h"
using namespace std;

//constructor
Bot::Bot()
{
    miniMax = MiniMax(&state);
	aStar = AStar(&state);
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
	aStar.AStarSearch(start, goal, cameFrom, costSoFar);
	const std::vector<Location> res = aStar.ReconstructPath(start, goal, cameFrom);
	//state.bug << "AStar: " << res.size() << endl;
	if (!res.empty()) {
		//state.bug << "next move: " << res.front() << endl; // Only if goal =/= start and goal not water.
	}
	for (auto from : res)
	{
		//state.bug << from << endl;
	}

	// TODO: Remove this test.
	const Location startBFS{ 30, 19 }, goalBFS{ 31, 10 };
	const auto resBFS = aStar.BreadthFirstSearch(startBFS, goalBFS);
	//state.bug << "BFS: " << resBFS.size() << endl;
	std::vector<Location> foodLocs;
	for (std::pair<Location, Location> from : resBFS)
	{
		if (state.grid[from.first.row][from.first.col].isFood)
		{
			foodLocs.push_back(from.first);
			//state.bug << from.first << endl;
		}
	}
	//state.bug << "food amount:" << foodLocs.size() << endl;

	// TODO: Remove this test.
	const Location startBFS2{ 9, 16 }, goalBFS2{ 15, 16 };
	const auto resBFS2 = aStar.BreadthFirstSearch(startBFS2, goalBFS2, ENEMYANT, 5);
	//state.bug << "BFS2: " << resBFS2.size() << endl;
	for (std::pair<Location, Location> from : resBFS2)
	{
		// state.bug << "enemies near " << from.first << endl;
	}

	state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
}

void Bot::setup()
{
    orders.clear();
    targets.clear();
    miniMax.myAnts.clear();
    miniMax.enemyAnts.clear();

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
	// find ant close to food
	for (const Location foodLoc : state.food)
	{
		// TODO: Make BFS radius smaller to reduce time taken?
		const Location start = foodLoc, goal{ state.getLocation(foodLoc, 2, 5) };
		const auto antLoc = aStar.BreadthFirstSearch(start, goal, MYANT);

		for (std::pair<Location, Location> from : antLoc)
		{
			const Location myAnt = from.first, enemiesRadius{ state.getLocation(myAnt, 2, 3) }, enemyRadius{ state.getLocation(foodLoc, 2, 2) };
			// Prioritize own food over enemy food.
			const auto isEnemyNearFood = aStar.BreadthFirstSearch(foodLoc, enemyRadius, ENEMYANT, 5).size() == 1;
			// Don't prioritize if enemies nearby.
			const auto enemiesCount = aStar.BreadthFirstSearch(myAnt, enemiesRadius, ENEMYANT, 2).size();

			string info = "gather food";

			// Enemy near food AND close enough to food.
			const auto dist = state.EuclideanDistance(myAnt, foodLoc);
			const bool acceptTrade = (isEnemyNearFood && dist < 3);
			if (acceptTrade) info += " (trade ant to not loose food)";

			if (!acceptTrade || enemiesCount > 1) continue;

			// If ant has no task.
			const bool isAntBusyWithFood = containsValue(targets, myAnt);
			if (!isAntBusyWithFood)
			{
				std::map<Location, Location> cameFrom;
				std::map<Location, double> costSoFar;
				aStar.AStarSearch(myAnt, foodLoc, cameFrom, costSoFar);
				const std::vector<Location> res = aStar.ReconstructPath(myAnt, foodLoc, cameFrom);
				if (!res.empty()) {
					makeMove(myAnt, res.front(), info);

				}
			}
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
                if (makeMove(hillLoc, d, "unblock hills"))
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
                if (makeMove(antLoc, unseenLoc, "explore")) {
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
			// Don't prioritize if enemies nearby (3 for now)
			// TODO: This may need to be removed when changing strategy.
			const Location enemyRadius{ state.getLocation(antLoc, 2, 3) };
			const auto enemiesCount = aStar.BreadthFirstSearch(antLoc, enemyRadius, ENEMYANT, 2).size();

			if (enemiesCount == 2) continue;

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
        makeMove(antLoc, hillLoc, "attack hills");
    }
}

void Bot::attackAnts()
{
    // minimax: group ants by attacks
    std::vector<std::tuple<double, Location, Location>> attackGroups;
    for (Location enemyAnt : state.enemyAnts)
    {
        for (Location antLoc : state.myAnts)
        {
            const bool hasMove = containsValue(orders, antLoc);
            if (!hasMove) {
                auto dist = state.EuclideanDistance(antLoc, enemyAnt);
                if (dist <= 6) // TODO: or another number?
                {
                    auto position = find_if(attackGroups.begin(), attackGroups.end(),
                        [=](auto item)
                        {
                            return get< 1 >(item) == antLoc;
                        });

                    if (position != attackGroups.end())
                    {
                        // avoid duplicates somehow
                        //state.bug << "dup? " << antLoc << endl;
                        continue;
                    }

                    // TODO: restrict size of attack groups by location instead of by the size of the vector to avoid timeouts
                    // the vector should be split by zones to do that
                    if (attackGroups.size() > 5) continue;
                        
                    attackGroups.emplace_back(dist, antLoc, enemyAnt);
                }
            }
        }
    }

    for (auto res : attackGroups)
    {
        const Location antLoc = std::get<1>(res);
        const Location enemyLoc = std::get<2>(res);
        miniMax.myAnts.push_back(new Ant(antLoc));
        miniMax.enemyAnts.push_back(new Ant(enemyLoc));
        //state.bug << antLoc << " vs " << enemyLoc << endl;
    }

    miniMax.Max(0);
    for (const Ant* myAnt : miniMax.myAnts) {
        if (myAnt->bestDest != Location(-1, -1)) makeMove(myAnt->loc, myAnt->bestDest, "attack ants"); // else?
    }
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


bool Bot::makeMove(const Location& loc, const Location& dest, const string& from) {
    const std::vector<int> directions = state.getDirections(loc, dest);

    for (const int d : directions)
    {
	    if (makeMove(loc, d, from))
	    {
		    targets[dest] = loc;
		    return true;
	    }
    }

	return false;
};

bool Bot::makeMove(const Location& loc, const int direction, const string& from)
{
	const Location newLoc = state.getLocation(loc, direction);

	const bool canMakeMove = state.isUnoccupied(newLoc) && orders.count(newLoc) == 0;
	if (!canMakeMove)
	{
		return false;
	}

    state.makeMove(loc, direction);
    orders[newLoc] = loc;

    // debug
    if (!from.empty()) {
        state.bug << "move from " << from << ": " << loc << " (" << CDIRECTIONS[direction] << ") to " << newLoc << "\n";
    }

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
