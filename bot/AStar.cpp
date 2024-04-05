#include "AStar.h"

#include <map>
#include <set>

AStar::AStar() = default;

std::vector<Location> AStar::Neighbors(Location loc, bool isStart, Location goal) const
{
	// Second is direction.
	auto cmp = [](std::pair<Location, int> a, std::pair<Location, int> b) { return a.second < b.second; };
	std::set<std::pair<Location, int>, decltype(cmp)> results(cmp);

	for (int d = 0; d < TDIRECTIONS; d++)
	{
		Location next = _state->GetLocation(loc, d);
		if (isStart && !_state->IsUnoccupied(next)) {
			// Don't add this neighbor to be able to do the first move.
			// Tiles farther than start are okay because own ants will probably move.
			continue;
		}

		// Allows to search within a radius without minding water, useful for BFS.
		if (goal != Location{ -1, -1 } && next == goal && _state->grid[goal.row][goal.col].isWater)
		{
			results.insert(std::make_pair(next, d));
		}

		if (!_state->grid[next.row][next.col].isWater) {
			results.insert(std::make_pair(next, d));
		}
	}

	// Used to keep the same order as TDIRECTIONS,
	// so that the sort order is right.
	std::vector<Location> locations;
	std::transform(std::begin(results), std::end(results),
		std::back_inserter(locations),
		[](auto const& pair) { return pair.first; });
	return locations;
}

void AStar::AStarSearch(Location start, Location goal, std::map<Location, Location>& cameFrom, std::map<Location, double>& costSoFar) const
{
	if (goal == start) {
		_state->bug << "Destination already reached (goal equals start)" << start << std::endl;
		return;
	}
	if (_state->grid[goal.row][goal.col].isWater) {
		_state->bug << "Destination is not a valid target (water tile) " << goal << std::endl;
		return;
	}

	PriorityQueue<Location, double> frontier;
	frontier.put(start, 0);

	cameFrom[start] = start;
	costSoFar[start] = 0;

	while (!frontier.empty()) {
		if (_state->TimeRemaining() < 200) {
			_state->bug << "A* timeout" << std::endl;
			// Don't send bad results
			cameFrom.clear();
			costSoFar.clear();
			break;
		}

		Location current = frontier.get();

		if (current == goal) {
			break;
		}

		std::vector<Location> neighbors;
		if (current == start)
		{
			neighbors = Neighbors(current, true);
		}
		else
		{
			neighbors = Neighbors(current, false);
		}

		for (Location next : neighbors) {
			// Here are our gcosts.
			const double newCost = costSoFar[current] + _state->ManhattanDistance(current, next);

			// Required to be able to visit nex nodes when we check new_cost < costSoFar[next].
			if (costSoFar[next] == 0.f)
			{
				costSoFar[next] = 9999.f;
			}

			if (costSoFar.find(next) == costSoFar.end() || newCost < costSoFar[next]) {
				costSoFar[next] = newCost;
				// ManhattanDistance is our heuristic/ hcost.
				// Priority is our fcost.
				const double priority = newCost + _state->ManhattanDistance(next, goal);
				frontier.put(next, priority);
				cameFrom[next] = current;
			}
		}
	}
}

std::vector<Location> AStar::ReconstructPath(Location start, Location goal, std::map<Location, Location> cameFrom, bool withStart) const {
	std::vector<Location> path;
	Location current = goal;

	if (cameFrom.find(goal) == cameFrom.end()) {
		_state->bug << " No path can be found" << std::endl;
		return path;
	}

	while (current != start) {
		if (_state->TimeRemaining() < 200) {
			_state->bug << "A* reconstruct timeout" << std::endl;
			break;
		}
		path.push_back(current);
		current = cameFrom[current];
	}

	if (withStart) {
		path.push_back(start);
	}

	std::reverse(path.begin(), path.end());
	return path;
}

std::vector<Location> AStar::BreadthFirstSearch(Location start, Location goal, SquareType searchForType, int count) const {
	std::map<Location, Location> cameFrom;
	std::vector<Location> foundLocs;
	if (goal == start) {
		_state->bug << "Destination already reached (goal equals start)" << start << std::endl;
		return {};
	}

	// No check for water here to allow to search within a radius without minding water.
	//if (_state->grid[goal.row][goal.col].isWater) {
	//	_state->bug << "Destination is not a valid target (water tile) " << goal << std::endl;
	//	return {};
	//}

	std::queue<Location> frontier;
	frontier.push(start);

	cameFrom[start] = start;

	int found = 0;
	while (!frontier.empty()) {
		if (_state->TimeRemaining() < 200) {
			_state->bug << "BFS timeout" << std::endl;
			// Don't send bad results
			cameFrom.clear();
			break;
		}

		Location current = frontier.front();
		frontier.pop();

		if (current == goal) {
			break;
		}

		for (auto next : Neighbors(current, false, goal)) {
			if (cameFrom.find(next) == cameFrom.end()) {
				// Ugly but working multiple conds, still not generic.
				if (searchForType != UNKNOWN) {
					if ((searchForType == MYANT && _state->grid[next.row][next.col].isMyAnt)
						|| (searchForType == HILL && _state->grid[next.row][next.col].isHill)
						|| (searchForType == FOOD && _state->grid[next.row][next.col].isFood)
						|| (searchForType == ENEMYANT && _state->grid[next.row][next.col].isEnemyAnt))
					{
						// Keep track of found locations in a dedicated array.
						found++;
						foundLocs.push_back(next);

						// Stop BFS when needed.
						if (found == count) {
							return foundLocs;
						}
					}
				}

				// BFS as usual.
				frontier.push(next);
				cameFrom[next] = current;
			}
		}
	}

	if (searchForType != UNKNOWN)
	{
		return foundLocs;
	}

	std::vector<Location> locations;
	std::transform(std::begin(cameFrom), std::end(cameFrom),
		std::back_inserter(locations),
		[](auto const& pair) { return pair.first; });
	return locations;
}