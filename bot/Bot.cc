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
	// TODO: priority?
	// for now it is at the top because we only have one hill on the map chosen for the contest so we don't want to loose it
	DefendHills();
    GatherFood();
    AttackHills();
    AttackAnts();
	TrackEnemies();
	EscapeEnemies();
	// explore unseen areas
	ExploreMap();
	RandomMove();
	UnblockHills();

	// TODO: Remove this test.
	const Location start{ 0, 28 }, goal{ 27, 10 };
	std::map<Location, Location> cameFrom;
	std::map<Location, double> costSoFar;
	_aStar.AStarSearch(start, goal, cameFrom, costSoFar, _orders);
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
	for (Location from : resBFS)
	{
		if (_state.grid[from.row][from.col].isFood)
		{
			foodLocs.push_back(from);
			//_state.bug << from << endl;
		}
	}
	//_state.bug << "food amount:" << foodLocs.size() << endl;

	// TODO: Remove this test.
	//for (int i = 0; i < 1000; ++i)
	//{
		const Location startBFS2{ 9, 16 }, goalBFS2{ 15, 16 };
		const auto resBFS2 = _aStar.BreadthFirstSearch(startBFS2, goalBFS2, ENEMYANT, 5);
		//_state.bug << "BFS2: " << resBFS2.size() << endl;
		for (Location from : resBFS2)
		{
			// _state.bug << "enemies near " << from << endl;
		}
	//}
	

	_state.bug << "time taken: " << _state.timer.GetTime() << "ms" << endl << endl;
}

void Bot::Setup()
{
    _orders.clear();
    _targets.clear();
    _miniMax.myAnts.clear();
    _miniMax.enemyAnts.clear();

	// prevent stepping on own hill
	// necessary for Bot::UnblockHills
	for (Location myHill : _state.myHills)
	{
		_orders[myHill] = Location(-1, -1);
	}

	_attackGroups.clear();

	// Add all locations to unseen tiles set.
	// Necessary for Bot::ExploreMap, run once per game.
	if (_unseenTiles.empty()) {
		for (int row = 0; row < _state.rows; row++) {
			for (int col = 0; col < _state.cols; col++) {
				auto from = Location(row, col);
				_unseenTiles[from] = 0;
			}
		}
	}

	// Remove any tiles that can be seen, run each turn.
	for (auto iter = _unseenTiles.begin(); iter != _unseenTiles.end();)
	{
		if (_state.grid[iter->first.row][iter->first.col].isVisible)
		{
			iter = _unseenTiles.erase(iter);
			//_state.bug << "seen: " << iter->row << " " << iter->col << "\n";
		}
		else
		{
			++iter;
		}
	}

	// Increase the "last visited" counter.
	for (auto& res : _unseenTiles)
	{
		res.second++;
}
}

void Bot::GatherFood()
{
	// find ant close to food
	for (const Location foodLoc : _state.food)
	{
		// TODO: Make BFS radius smaller to reduce time taken?
		const Location start = foodLoc, goal{ _state.GetLocation(foodLoc, 2, 5) };
		const auto antLoc = _aStar.BreadthFirstSearch(start, goal, MYANT);

		for (const Location from : antLoc)
		{
			const Location myAnt = from, enemiesRadius{ _state.GetLocation(myAnt, 2, 3) }, enemyRadius{ _state.GetLocation(foodLoc, 2, 2) };
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
				_aStar.AStarSearch(myAnt, foodLoc, cameFrom, costSoFar, _orders);
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

/**
 * \brief Explore the map using a "last visited" counter, to always pick the oldest seen location.
 */
void Bot::ExploreMap()
{
	for (Location antLoc : _state.myAnts)
	{
		const bool hasMove = ContainsValue(_orders, antLoc);
		if (!hasMove)
		{
			// Look for unexplored locations that are relatively close.
			// 1) By checking !Square::isVisible, add location to the unseenDist array if it is out of sight.
			const Location goal{ _state.GetLocation(antLoc, 1, 15) };
			const auto resBFS = _aStar.BreadthFirstSearch(antLoc, goal, INVISIBLE, 3);
			std::vector<std::tuple<int, Location>> unseenDist;
			for (Location from : resBFS)
			{
				auto dist = _state.ManhattanDistance(antLoc, from);
				unseenDist.emplace_back(dist, from);
				// avoid timeout, even if the search is not complete
				// better than being removed from the game
				if (_state.TimeRemaining() < 200) {
					_state.bug << "explore timeout" << endl;
					break;
				}
			}

			// 2) Find the greatest "last visited" path we can go to, stop when we managed to move in that direction.
			std::sort(unseenDist.begin(), unseenDist.end(), greater<>());
			for (auto res : unseenDist)
			{
				Location unseenLoc = std::get<1>(res);
				auto& lastVisited = _unseenTiles[unseenLoc];
				// We can know if it really wasn't visited recently or if it was just out of sight (lastVisited == 0). 
				if (lastVisited != 0) {
					std::map<Location, Location> cameFrom;
					std::map<Location, double> costSoFar;
					_aStar.AStarSearch(antLoc, unseenLoc, cameFrom, costSoFar, _orders);
					const std::vector<Location> nextMove = _aStar.ReconstructPath(antLoc, unseenLoc, cameFrom);
					if (!nextMove.empty()) {
						Location nextDest = nextMove.front();
						if (MakeMove(antLoc, nextDest, "explore")) {
							lastVisited = 0;
							break;
						}
					}
				}
			}
		}
	}
}

/**
 * \brief To use if there is nothing else to do, even if the timeout threshold is triggered because this code should be fast enough.
 */
void Bot::RandomMove()
{
	for (Location antLoc : _state.myAnts)
	{
		const bool hasMove = ContainsValue(_orders, antLoc);
		if (!hasMove)
		{
			const Location& hill = _state.myHills[0];
			const float dist = _state.ManhattanDistance(antLoc, hill);
			// Only leave hill if close to it.
			if (dist < 10) {
				// TODO: For more than one hill maps.
				const std::vector<int> directions = _state.GetOppositeDirections(antLoc, hill);

				for (const int d : directions)
				{
					if (MakeMove(antLoc, d, "leave hill"))
					{
						break;
					}
				}
			}

			int tryCount = 0;
			while (tryCount < 4) {
				const int direction = rand() % 4;
				if (MakeMove(antLoc, direction, "random move"))
				{
					break;
				}
				tryCount++;
			}
		}
	}
}

void Bot::AttackHills()
{
	// Do not attack hills at the start of the game (or when too weak), even if we can see/ win another hill.
	if (_state.myAnts.size() < 10) return;

	// Keep track of hills in Bot because if we lose vision of the hill, we don't want to forget that it's there (how State::enemyHills works).
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

		for (const Location myAnt : antLoc)
		{
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
				_aStar.AStarSearch(myAnt, hillLoc, cameFrom, costSoFar, _orders);
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
    }

    _miniMax.Max(0);
    for (const Ant* myAnt : _miniMax.myAnts) {
        if (myAnt->bestDest != Location(-1, -1)) MakeMove(myAnt->loc, myAnt->bestDest, "attack ants"); // else?
    }
}

void Bot::DefendHills()
{
	for (Location myHill : _state.myHills)
	{
		// Look for enemies close to hill.
		const Location start = myHill, goal{ _state.GetLocation(myHill, 2, 10) };
		const auto enemiesLoc = _aStar.BreadthFirstSearch(start, goal, ENEMYANT, 5);
		for (Location enemyAnt : enemiesLoc)
		{
			// Look for a single own ant to defend the hill: trade.
			const Location myAnts{ _state.GetLocation(myHill, 2, 20) };
			// Five ants, not just one, to fix the problem of always trying the same ant move.
			const auto antLoc = _aStar.BreadthFirstSearch(enemyAnt, myAnts, MYANT, 5);
			for (Location myAnt: antLoc)
			{
				// TODO: Check for hasMove inside BFS to avoid trying to always make the same ant move...
				// If ant has no task.
				const bool hasMove = ContainsValue(_orders, myAnt);
				if (!hasMove)
				{
					std::map<Location, Location> cameFrom;
					std::map<Location, double> costSoFar;
					_aStar.AStarSearch(myAnt, enemyAnt, cameFrom, costSoFar, _orders);
					const std::vector<Location> res = _aStar.ReconstructPath(myAnt, enemyAnt, cameFrom);
					if (!res.empty()) {
						MakeMove(myAnt, res.front(), "defend hill");
					}
				}
			}
		}
	}
}

/**
 * \brief Ants without a move that are close to attacking ants will try to join the fight next turn if they are not too far.
 */
void Bot::TrackEnemies()
{
	for (auto group : _attackGroups)
	{
		const Location myAnt = std::get<1>(group);
		const Location enemyAnt = std::get<2>(group);
		// Attack happened for real (sometimes minimax doesn't find a winning destination for a fight so the ant doesn't attack).
		const bool isAttacking = ContainsValue(_orders, myAnt);
		if (isAttacking) 
		{
			// Look for friends close to ant.
			const Location myAnts{ _state.GetLocation(myAnt, 2, 3) };
			const auto antsLoc = _aStar.BreadthFirstSearch(myAnt, myAnts, MYANT, 5);
			for (auto loc : antsLoc)
			{
				// If ant has no task.
				const bool hasMove = ContainsValue(_orders, loc);
				if (!hasMove)
				{
					std::map<Location, Location> cameFrom;
					std::map<Location, double> costSoFar;
					_aStar.AStarSearch(loc, enemyAnt, cameFrom, costSoFar, _orders);
					const std::vector<Location> res = _aStar.ReconstructPath(loc, enemyAnt, cameFrom);
					if (!res.empty()) 
					{
						MakeMove(loc, res.front(), "track enemy");
					}
				}
			}
		}
	}
}

/**
 * \brief Ants without a move that are close to attacking ants will try to join the fight next turn if they are not too far.
 */
void Bot::EscapeEnemies()
{
	for (Location myAnt : _state.myAnts)
	{
		const bool hasMove = ContainsValue(_orders, myAnt);
		if (!hasMove)
		{
			//TODO: instead of two BFS, we could need only one because the search radius is the same.
			// Look for friends close to ant.
			const Location myAnts{ _state.GetLocation(myAnt, 2, 5) };
			const auto antsLoc = _aStar.BreadthFirstSearch(myAnt, myAnts, MYANT, 5);

			// Look for enemies close to ant.
			const Location enemies{ _state.GetLocation(myAnt, 2, 5) };
			const auto enemiesLoc = _aStar.BreadthFirstSearch(myAnt, enemies, ENEMYANT, 5);

			// Not enough friends are close and there is more than one enemy so we can't trade.
			if (antsLoc.size() < 5 && enemiesLoc.size() > 1) 
			{
				const std::vector<int> directions = _state.GetOppositeDirections(myAnt, enemiesLoc[0]);
				for (const int d : directions)
				{
					if (MakeMove(myAnt, d, "escape"))
					{
						break;
					}
				}
			}
		}
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

	// This check is allowed here because the time complexity is O(log N) and N is max 5 in our case,
	// so it is always small and can only get smaller as the game progress.
	const bool isHillRazed = _enemyHills.find(r_loc) != _enemyHills.end();
	if (isHillRazed)
	{
		_enemyHills.erase(r_loc);
	}

	const bool canMakeMove = _state.IsUnoccupied(_orders, newLoc);
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
