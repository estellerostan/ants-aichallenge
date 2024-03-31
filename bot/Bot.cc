#include "Bot.h"
#include "AStar.h"
using namespace std;

//constructor
Bot::Bot()
{
    _miniMax = MiniMax(&_state);
	_aStar = AStar(&_state);
};

//plays a single game of Ants.
void Bot::PlayGame()
{
	//reads the game parameters and sets up
	cin >> _state;
	_state.Setup();
	EndTurn();

	//continues making moves while the game is not over
	while (cin >> _state)
	{
		_state.UpdateVisionInformation();
		MakeMoves();
		EndTurn();
	}
}

//makes the bots moves for the turn
void Bot::MakeMoves()
{
	_state.bug << "turn " << _state.turn << ":" << endl;
	_state.bug << _state << endl;

    Setup();
    GatherFood();
    AttackHills();
    AttackAnts();
	// TODO: fix perf problems for large maps (of 6p with time taken > 100ms)
	// explore unseen areas
	ExploreMap();
	UnblockHills();

	// TODO: Remove this test.
	const Location start{ 0, 28 }, goal{ 27, 10 };
	std::map<Location, Location> cameFrom;
	std::map<Location, double> costSoFar;
	_aStar.AStarSearch(start, goal, cameFrom, costSoFar);
	const std::vector<Location> res = _aStar.ReconstructPath(start, goal, cameFrom);
	//_state.bug << "AStar: " << res.size() << endl;
	if (!res.empty()) {
		//_state.bug << "next move: " << res.front() << endl; // Only if goal =/= start and goal not water.
	}
	for (auto from : res)
	{
		//_state.bug << from << endl;
	}

	// TODO: Remove this test.
	const Location startBFS{ 30, 19 }, goalBFS{ 31, 10 };
	const auto resBFS = _aStar.BreadthFirstSearch(startBFS, goalBFS);
	//_state.bug << "BFS: " << resBFS.size() << endl;
	std::vector<Location> foodLocs;
	for (std::pair<Location, Location> from : resBFS)
	{
		if (_state.grid[from.first.row][from.first.col].isFood)
		{
			foodLocs.push_back(from.first);
			//_state.bug << from.first << endl;
		}
	}
	//_state.bug << "food amount:" << foodLocs.size() << endl;

	// TODO: Remove this test.
	const Location startBFS2{ 9, 16 }, goalBFS2{ 15, 16 };
	const auto resBFS2 = _aStar.BreadthFirstSearch(startBFS2, goalBFS2, ENEMYANT, 5);
	//_state.bug << "BFS2: " << resBFS2.size() << endl;
	for (std::pair<Location, Location> from : resBFS2)
	{
		// _state.bug << "enemies near " << from.first << endl;
	}

	_state.bug << "time taken: " << _state.timer.GetTime() << "ms" << endl << endl;
}

void Bot::Setup()
{
    _orders.clear();
    _targets.clear();
    _miniMax.myAnts.clear();
    _miniMax.enemyAnts.clear();

	// add all locations to unseen tiles set, run once
	// necessary for Bot::ExploreMap
	if (_unseenTiles.empty()) {
		for (int row = 0; row < _state.rows; row++) {
			for (int col = 0; col < _state.cols; col++) {
				_unseenTiles.insert(Location(row, col));
			}
		}
	}
	// remove any tiles that can be seen, run each turn
	for (auto iter = _unseenTiles.begin(); iter != _unseenTiles.end();)
	{
		if (_state.grid[iter->row][iter->col].isVisible)
		{
			iter = _unseenTiles.erase(iter);
			//_state.bug << "seen: " << iter->row << " " << iter->col << "\n";
		}
		else
		{
			++iter;
		}
	}

	// prevent stepping on own hill
	// necessary for Bot::UnblockHills
	for (Location myHill : _state.myHills)
	{
		_orders[myHill] = Location(-1, -1);
	}

	_attackGroups.clear();
}

void Bot::GatherFood()
{
	// find ant close to food
	for (const Location foodLoc : _state.food)
	{
		// TODO: Make BFS radius smaller to reduce time taken?
		const Location start = foodLoc, goal{ _state.GetLocation(foodLoc, 2, 5) };
		const auto antLoc = _aStar.BreadthFirstSearch(start, goal, MYANT);

		for (std::pair<Location, Location> from : antLoc)
		{
			const Location myAnt = from.first, enemiesRadius{ _state.GetLocation(myAnt, 2, 3) }, enemyRadius{ _state.GetLocation(foodLoc, 2, 2) };
			// Prioritize own food over enemy food.
			const auto isEnemyNearFood = _aStar.BreadthFirstSearch(foodLoc, enemyRadius, ENEMYANT, 5).size() == 1;
			// Don't prioritize if enemies nearby.
			const auto enemiesCount = _aStar.BreadthFirstSearch(myAnt, enemiesRadius, ENEMYANT, 2).size();

			string info = "gather food";

			// Enemy near food AND close enough to food.
			const auto dist = _state.ManhattanDistance(myAnt, foodLoc);
			const bool acceptTrade = (isEnemyNearFood && dist < 3);
			if (acceptTrade) info += " (trade ant to not loose food)";

			if (!(acceptTrade || enemiesCount < 2)) {
				continue;
			}

			// If ant has no task.
			const bool isAntBusyWithFood = ContainsValue(_targets, myAnt);
			if (!isAntBusyWithFood)
			{
				std::map<Location, Location> cameFrom;
				std::map<Location, double> costSoFar;
				_aStar.AStarSearch(myAnt, foodLoc, cameFrom, costSoFar);
				const std::vector<Location> res = _aStar.ReconstructPath(myAnt, foodLoc, cameFrom);
				if (!res.empty()) {
					MakeMove(myAnt, res.front(), info);
				}
			}
		}
	}
}

void Bot::UnblockHills()
{
    for (Location hillLoc : _state.myHills)
    {
        auto it = std::find(_state.myAnts.cbegin(), _state.myAnts.cend(), hillLoc);
        const bool hasMove = ContainsValue(_orders, hillLoc);
        if (it != _state.myAnts.end() && !hasMove)
        {
            // an ant is on a hill
            for (int d = 0; d < TDIRECTIONS; d++)
            {
                if (MakeMove(hillLoc, d, "unblock hills"))
                {
                    break;
                }
            }
        }
    }
}

void Bot::ExploreMap()
{
    for (Location antLoc : _state.myAnts)
    {
        const bool hasMove = ContainsValue(_orders, antLoc);
        if (!hasMove)
        {
            std::vector<std::tuple<int, Location>> unseenDist;
            for (Location unseenLoc : _unseenTiles)
            {
                auto dist = _state.EuclideanDistance(antLoc, unseenLoc);
                unseenDist.emplace_back(dist, unseenLoc);
                // avoid timeout, even if the search is not complete
                // better than being removed from the game
				if (_state.TimeRemaining() < 200) {
					_state.bug << "explore timeout" << endl;
					break;
				}
            }
            std::sort(unseenDist.begin(), unseenDist.end());
            for (auto res : unseenDist) 
            {
                Location unseenLoc = std::get<1>(res);
                if (MakeMove(antLoc, unseenLoc, "explore")) {
                    break;
                }
            }
        }
    }
}

void Bot::AttackHills()
{
	// Do not attack hills at the start of the game (or when too weak), even if we can see/ win another hill.
	if (_state.myAnts.size() < 10) return;

	for (auto enemyHill : _state.enemyHills)
	{
		if (_enemyHills.count(enemyHill) == 0)
		{
			_enemyHills.insert(enemyHill);
		}
	}

	// find ant close to hills
	// in the same way Bot::GatherFood works.
	for (const auto hillLoc : _enemyHills)
	{
		const Location start = hillLoc, goal{ _state.GetLocation(hillLoc, 2, 20) };
		const auto antLoc = _aStar.BreadthFirstSearch(start, goal, MYANT, 5);

		for (std::pair<Location, Location> from : antLoc)
		{
			const Location myAnt = from.first;

			// Don't prioritize if enemies nearby (5 for now) and if there is more than one ant attacking the hill
			// For example: one lonesome ant can try to attack a hill (worst case: trade, huge win possible),
			//			    but a group should focus on attacking enemies first to minimize the loss of own ants (they all focus on the same task).
			// TODO: This may need to be removed when changing strategy.
			const Location enemyRadius{ _state.GetLocation(myAnt, 2, 5) };
			const auto enemiesCount = _aStar.BreadthFirstSearch(myAnt, enemyRadius, ENEMYANT, 2).size();

			if (antLoc.size() > 1 && enemiesCount > 0) {
				continue;
			}

			// If ant has no task.
			const bool hasMove = ContainsValue(_orders, myAnt);
			if (!hasMove)
			{
				std::map<Location, Location> cameFrom;
				std::map<Location, double> costSoFar;
				_aStar.AStarSearch(myAnt, hillLoc, cameFrom, costSoFar);
				const std::vector<Location> res = _aStar.ReconstructPath(myAnt, hillLoc, cameFrom);
				if (!res.empty()) {
					MakeMove(myAnt, res.front(), "attack hills");
				}
			}
		}
	}
}

void Bot::CreateAttackGroups()
{
    // minimax: group ants by attacks
    std::vector<std::tuple<double, Location, Location>> attackGroups;
    for (Location enemyAnt : _state.enemyAnts)
    {
        for (Location antLoc : _state.myAnts)
        {
            const bool hasMove = ContainsValue(_orders, antLoc);
            if (!hasMove) {
                auto dist = _state.EuclideanDistance(antLoc, enemyAnt);
                if (dist <= 6) // TODO: or another number?
                {
                    auto position = find_if(_attackGroups.begin(), _attackGroups.end(),
                        [=](auto item)
                        {
                            return get< 1 >(item) == antLoc;
                        });

                    if (position != _attackGroups.end())
                    {
                        // avoid duplicates somehow
                        //_state.bug << "dup? " << antLoc << endl;
                        continue;
                    }

                    // TODO: restrict size of attack groups by location instead of by the size of the vector to avoid timeouts
                    // the vector should be split by zones to do that
                    if (_attackGroups.size() > 5) continue;
                        
                    _attackGroups.emplace_back(dist, antLoc, enemyAnt);
                }
            }
        }
    }
    }

void Bot::AttackAnts()
{
	CreateAttackGroups();
    for (auto res : _attackGroups)
    {
        const Location antLoc = std::get<1>(res);
        const Location enemyLoc = std::get<2>(res);
        _miniMax.myAnts.push_back(new Ant(antLoc));
        _miniMax.enemyAnts.push_back(new Ant(enemyLoc));
        //_state.bug << antLoc << " vs " << enemyLoc << endl;
    }

    _miniMax.Max(0);
    for (const Ant* myAnt : _miniMax.myAnts) {
        if (myAnt->bestDest != Location(-1, -1)) MakeMove(myAnt->loc, myAnt->bestDest, "attack ants"); // else?
    }
}

bool Bot::ContainsValue(std::map<Location, Location>& r_locMap, const Location& r_antLoc)
{
	if (r_locMap.empty())
	{
		return false;
	}

	for (auto it = r_locMap.begin(); it != r_locMap.end(); ++it)
	{
		if (it->second == r_antLoc)
		{
			return true;
		}
	}
	return false;
}


bool Bot::MakeMove(const Location& r_loc, const Location& r_dest, const string& r_from) {
    const std::vector<int> directions = _state.GetDirections(r_loc, r_dest);

    for (const int d : directions)
    {
	    if (MakeMove(r_loc, d, r_from))
	    {
		    _targets[r_dest] = r_loc;
		    return true;
	    }
    }

	return false;
};

bool Bot::MakeMove(const Location& r_loc, const int direction, const string& r_from)
{
	const Location newLoc = _state.GetLocation(r_loc, direction);

	const bool canMakeMove = _state.IsUnoccupied(newLoc) && _orders.count(newLoc) == 0;
	if (!canMakeMove)
	{
		return false;
	}

    _state.MakeMove(r_loc, direction);
    _orders[newLoc] = r_loc;

    // debug
    if (!r_from.empty()) {
        _state.bug << "move from " << r_from << ": " << r_loc << " (" << CDIRECTIONS[direction] << ") to " << newLoc << "\n";
    }

    return true;
};

//finishes the turn
void Bot::EndTurn()
{
	if (_state.turn > 0)
		_state.Reset();
	_state.turn++;

	cout << "go" << endl;
};
