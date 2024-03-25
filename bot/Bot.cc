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
	auto res2 = aStar.BreadthFirstSearch(startBFS, goalBFS);
	state.bug << "BFS: " << res2.size() << endl;
	std::vector<Location> foodLocs;
	for (std::pair<Location, Location> from : res2)
	{
		if (state.grid[from.first.row][from.first.col].isFood)
		{
			foodLocs.push_back(from.first);
			state.bug << from.first << endl;
		}
	}
	state.bug << "food amount:" << foodLocs.size() << endl;
	
	state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
}

void Bot::setup()
{
	orders.clear();
	targets.clear();

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
