/*
 Adapted from:
	 Sample code from https://www.redblobgames.com/pathfinding/a-star/
	 Copyright 2014 Red Blob Games <redblobgames@gmail.com>

	 License: Apache v2.0 <http://www.apache.org/licenses/LICENSE-2.0.html>
*/

#include "AStar.h"

#include <map>
#include <set>

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

/**
 * \brief Find the shortest path from source to goal.
 * \param start Start location
 * \param goal Goal location
 * \param cameFrom Store the location of where we came for AStar::ReconstructPath
 * \param costSoFar
 */
void AStar::AStarSearch(Location start, Location goal, std::map<Location, Location>& cameFrom, std::map<Location, double>& costSoFar) const
{
	if (goal == start) {
		_state->bug << "Destination already reached (goal equals start)" << std::endl;
		return;
	}
	if (_state->grid[goal.row][goal.col].isWater) {
		_state->bug << "Destination is not a valid target (water tile)" << std::endl;
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

		for (Location next : neighbors) { // TODO: isStart
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

/**
 * \brief Returns the path to go from start to goal
 * \param start Start location
 * \param goal Goal location
 * \param cameFrom The location of where we came stored in AStar::Search
 * \param withStart Optional: here we directly want the next move to use it in makeMove.
 * \return
 */
std::vector<Location> AStar::ReconstructPath(Location start, Location goal, std::map<Location, Location> cameFrom, bool withStart) const {
	std::vector<Location> path;
	Location current = goal;

	if (cameFrom.find(goal) == cameFrom.end()) {
		return path; // No path can be found.
	}

	while (!(current == start)) { // TODO: !=
		if (_state->timeRemaining() < 200) {
			_state->bug << "A* reconstruct timeout " << std::endl;
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

/**
 * \brief Explore the map with early exit
 * \param start Start location
 * \param goal Goal location
 * \param searchForType Allows to stop the search when the condition to find a type is met, even if the goal is not reached
 * \param count How many locations of a type should be found before stopping the search
 */
std::map<Location, Location> AStar::BreadthFirstSearch(Location start, Location goal, SquareType searchForType) const {
	std::map<Location, Location> cameFrom;
	if (goal == start) {
		_state->bug << "Destination already reached (goal equals start)" << std::endl;
		return cameFrom;
	}
	if (_state->grid[goal.row][goal.col].isWater) {
		_state->bug << "Destination is not a valid target (water tile)" << std::endl;
		return cameFrom;
	}

	std::queue<Location> frontier;
	frontier.push(start);

	cameFrom[start] = start;

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
						cameFrom.clear();
						cameFrom[next] = current;
						return cameFrom;
					}
				}

				frontier.push(next);
				cameFrom[next] = current;
			}
		}
	}

	if (searchForType != UNKNOWN)
	{
		// No own ant was found so return an empty result.
		cameFrom.clear();
		return cameFrom;
	}

	return cameFrom;
}