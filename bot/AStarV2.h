#pragma once
#include <vector>
#include <unordered_map>
#include <map>
#include "State.h"

/**
* \brief Class AStarv2: contains a version (V2) of an A* algorithm and its associated function that computes paths between two locations.
*
*/
class AStarV2
{
public:

	/**
	 * \brief Constructor for the AStarV2 class.
	 * \param State& state reference to the current state of the game.
	 */
	explicit AStarV2(State& state) : _state(state) {}

	/**
	 * \brief Computes a path between two locations.
	 * \param Location startLocation the starting point of the path.
	 * \param Location destinationLocation the destination point of the path.
	 * \return Vector, containing the location points forming a path between startLocation and destinationLocation.
	 */
	std::vector<Location> GetPath(Location startLocation, Location destinationLocation);

private:

	//Reference & to the current state of the game.
	State& _state;

	/**
	 * \brief Computes the Manhattan distance between two locations. Takes into consideration the wrapped aspect of the map.
	 * \param Location a first location.
	 * \param Location b second location.
	 * \return Double, the Manhattan distance value between a and b.
	 */
	inline double Heuristic(Location a, Location b);

	/**
	 * \brief Computes the Manhattan distance between two locations.
	 * \param Location a first location.
	 * \param Location b second location.
	 * \return Float, the Manhattan distance value between a and b.
	 */
	inline double ManhattanDistance(Location a, Location b);

	/**
	 * \brief Checks if the Location given in parameter is a valid location (not a water tile).
	 * \param Location targetLocation location to check.
	 * \return Bool true if the location is valid, false otherwise.
	 */
	bool IsLocationValid(Location targetLocation);

	/**
	 * \brief Reconstruct the path between two locations.
	 * \param Location start the starting point of the path.
	 * \param Location goal the destination point of the path.
	 * \param map<Location, Location> cameFrom a map of how each points of the path are linked to each others.
	 * \return vector<Location>, containing the location points forming a path between start and goal.
	 */
	std::vector<Location> Reconstruct_path(Location start, Location goal, std::map<Location, Location> cameFrom);
	void A_star_search(Location start, Location goal, std::map<Location, Location>& cameFrom, std::map<Location, double>& costSoFarar);
};

