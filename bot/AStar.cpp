#include "AStar.h"

#include <map>
#include <set>

AStar::AStar() = default;

std::set<Location> AStar::Neighbors(Location loc, bool isStart) const
{
	std::set<Location> results;

	for (int d = 0; d < TDIRECTIONS; d++)
	{
		Location next = _state->getLocation(loc, d);
		if (isStart && !_state->isUnoccupied(next)) {
			// Don't add this neighbor to be able to do the first move.
			// Tiles farther than start are okay because own ants will probably move.
			continue;
		}
		if (!_state->grid[next.row][next.col].isWater) {
			results.insert(next);
		}
	}

	return results;
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
		if (_state->timeRemaining() < 200) {
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

		std::set<Location> neighbors;
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
		if (_state->timeRemaining() < 200) {
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

std::map<Location, Location> AStar::BreadthFirstSearch(Location start, Location goal, SquareType searchForType, int count) const {
	std::map<Location, Location> cameFrom;
	std::map<Location, Location> foundLocs;
	if (goal == start) {
		_state->bug << "Destination already reached (goal equals start)" << start << std::endl;
		return cameFrom;
	}

	// Allows to search within a radius without minding water.
	// TODO: Put back? what if we choose a goal and we step on a water tile by accident?
	//if (_state->grid[goal.row][goal.col].isWater) {
	//	_state->bug << "Destination is not a valid target (water tile) " << goal << std::endl;
	//	return cameFrom;
	//}

	std::queue<Location> frontier;
	frontier.push(start);

	cameFrom[start] = start;

	int found = 0;
	while (!frontier.empty()) {
		if (_state->timeRemaining() < 200) {
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

		for (auto next : Neighbors(current, false)) {
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
						foundLocs[next] = current;

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

	return cameFrom;
}