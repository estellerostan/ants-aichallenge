#pragma once
#include <vector>
#include <unordered_map>
#include <map>
#include "State.h"

class Pathfinding
{
public:
	explicit Pathfinding(State& state) : _state(state) {}

	std::vector<Location> GetPath(Location startLocation, Location destinationLocation);

private:
	State& _state;

	inline double ManhattanDistance(Location a, Location b);
};

