#pragma once
#include <vector>
#include <unordered_map>
#include <map>
#include "State.h"

/**
* \brief Class Pathfinding: contains an algorithm and its associated function that computes paths between two locations.
* 
*/
class Pathfinding
{
public:

	/**
	 * \brief Constructor for the Pathfinding class.
	 * \param State& state reference to the current state of the game.
	 */
	explicit Pathfinding(State& state) : _state(state) {}

	/**
	 * \brief Computes a path between two locations.
	 * \param Location startLocation the starting point of the path.
	 * \param Location destinationLocation the destination point of the path.
	 * \return vector<Location>, containing the location points forming a path between startLocation and destinationLocation.
	 */
	std::vector<Location> GetPath(Location startLocation, Location destinationLocation);

private:

	//Reference & to the current state of the game.
	State& _state;

	/**
	 * \brief Computes the Manhattan distance between two locations.
	 * \param Location a first location.
	 * \param Location b second location.
	 * \return Double, the Manhattan distance value between a and b.
	 */
	inline double ManhattanDistance(Location a, Location b);
};

