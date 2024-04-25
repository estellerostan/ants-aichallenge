/*
 Adapted from:
	 Sample code from https://www.redblobgames.com/pathfinding/a-star/
	 Copyright 2014 Red Blob Games <redblobgames@gmail.com>

	 License: Apache v2.0 <http://www.apache.org/licenses/LICENSE-2.0.html>
*/

#pragma once
#include <map>

#include "State.h"

class AStar
{
	State* _state;

	template<typename T, typename PriorityT>
	struct PriorityQueue {
		typedef std::pair<PriorityT, T> PQElement;
		std::priority_queue<PQElement, std::vector<PQElement>,
			std::greater<>> elements;

		bool empty() const {
			return elements.empty();
		}

		void put(T item, PriorityT priority) {
			elements.emplace(priority, item);
		}

		T get() {
			T bestItem = elements.top().second;
			elements.pop();
			return bestItem;
		}
	};

	std::vector<Location> Neighbors(Location loc, bool isStart, Location goal = Location{ -1, -1 }, std::map<Location, Location> orders = {{ Location{ -1, -1 },  Location{ -1, -1 }}}) const;

public:
	AStar();
	explicit AStar(State* state) : _state(state) {}

	/**
	 * \brief Find the shortest path from source to goal.
	 * \param start Start location
	 * \param goal Goal location
	 * \param cameFrom Store the location of where we came for AStar::ReconstructPath
	 * \param costSoFar
	 * \param orders Needs Bot::_orders to check for next moves (new) as well as current moves (classic)
	 */
	void AStarSearch(Location start, Location goal, std::map<Location, Location>& cameFrom, std::map<Location, double>& costSoFar, std::map<Location, Location>& orders) const;

	/**
	 * \brief Returns the path to go from start to goal
	 * \param start Start location
	 * \param goal Goal location
	 * \param cameFrom The location of where we came stored in AStar::Search
	 * \param withStart Optional: here we directly want the next move to use it in makeMove.
	 * \return All the locations forming a path from goal to start according to a map linking each location to its previous location, reversed.
	 */
	std::vector<Location> ReconstructPath(Location start, Location goal, std::map<Location, Location> cameFrom, bool withStart = false) const;

	/**
	 * \brief Explore the map with early exit
	 * \param start Start location
	 * \param goal Goal location
	 * \param searchForType Allows to stop the search when the condition to find a type is met, even if the goal is not reached
	 * \param count How many locations of a type should be found before stopping the search
	 * \return Either all the visited locations or the locations matching the wanted type if searchForType is set, with maximum count locations.
	 *		   If no match was found, return an empty result.
	 */
	std::vector<Location> BreadthFirstSearch(Location start, Location goal, SquareType searchForType = UNKNOWN, int count = 1) const;
};

