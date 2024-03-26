#pragma once
#include <map>
#include <set>

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

	std::set<Location> Neighbors(Location loc, bool isStart) const;

public:
	explicit AStar(State* state) : _state(state) {}

	void AStarSearch(Location start, Location goal, std::map<Location, Location>& cameFrom, std::map<Location, double>& costSoFar) const;

	std::vector<Location> ReconstructPath(Location start, Location goal, std::map<Location, Location> cameFrom, bool withStart = false) const;

	std::map<Location, Location> BreadthFirstSearch(Location start, Location goal, SquareType searchForType = UNKNOWN) const;
};

