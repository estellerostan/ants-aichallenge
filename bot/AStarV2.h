#pragma once
#include <vector>
#include <unordered_map>
#include <map>
#include "State.h"

class AStarV2
{
public:
	explicit AStarV2(State& state) : _state(state) {}

	std::vector<Location> GetPath(Location startLocation, Location destinationLocation);

private:
	State& _state;

	inline double Heuristic(Location a, Location b);
	inline double ManhattanDistance(Location a, Location b);
	bool IsLocationValid(Location targetLocation);
	std::vector<Location> reconstruct_path(Location start, Location goal, std::map<Location, Location> came_from);
	void a_star_search(Location start, Location goal, std::map<Location, Location>& came_from, std::map<Location, double>& cost_so_far);
};

