#ifndef BOT_H_
#define BOT_H_

#include "State.h"
#include "AStar.h"

#include <map>
#include <set>

#include "Ant.h"
#include "MiniMax.h"

/*
	This struct represents your bot in the game of Ants
*/
struct Bot
{
	Bot();
	void PlayGame();    //plays a single game of Ants

private:
	std::map<Location, Location> _orders;
	std::map<Location, Location> _targets;
	std::map<Location, int> _unseenTiles;
	std::set<Location> _enemyHills;
	std::vector<std::tuple<double, Location, Location>> _attackGroups;
	int exploreReach = 0;

	State _state;
	MiniMax _miniMax;
	AStar _aStar;

	/**
	 * \brief Makes the ant move for a single turn.
	 */
	void MakeMoves();

	/**
	 * \brief Checks if an ant is located somewhere on the map.
	 * \param r_locmap a map of all of the ants and their location
	 * \param r_antLoc the specific ant we want to check for
	 * \return bool true if the ant can be found, false otherwise.
	 */
	static bool ContainsValue(std::map<Location, Location>& r_locMap, const Location& r_antLoc);

	void Setup();

	/**
	 * \brief If there is food in a certain radius around an ant, make its task to gather the food.
	 */
	void GatherFood();

	/**
	 * \brief Makes sure the ants leave the hill on their turn after spawning to make sure they are not blocking it.
	 */
	void UnblockHills();

	/**
	 * \brief If an ant does not have any task to accomplish, make it explore unseen locations.
	 */
	void ExploreMap();

	/**
	* \brief To use if there is nothing else to do, even if the timeout threshold is triggered because this code should be fast enough.
	*/
	void RandomMove();

	/**
	* \brief Defines the ants strategy considering the enemy ant hills.
	*/
	void AttackHills();

	/**
	* \brief Defines the ants strategy regarding the formation of groups dedicated to attack other ants.
	*/
	void CreateAttackGroups();

	/**
	* \brief Defines the ants strategy regarding the extermination of enemy ants.
	*/
	void AttackAnts();

	/**
	* \brief Defines the ants strategy when defending their hill from enemy ants.
	*/
	void DefendHills();

	/**
	* \brief Ants without a move that are close to attacking ants will try to join the fight next turn if they are not too far.
	*/
	void TrackEnemies();

	/**
	 * \brief Ants without a move that are close to attacking ants will try to join the fight next turn if they are not too far.
	 */
	void EscapeEnemies();

	/**
	 * \brief Makes a move following a destination for a single ant.
	 * \param r_loc the current location
	 * \param r_dest the destination location
	 * \param r_from the maneuver that is being executed
	 * \return bool true if the ant can make a move towards the destination, false otherwise.
	 */
	bool MakeMove(const Location& r_loc, const Location& r_dest, const std::string& r_from = {});
	/**
	 * \brief Makes a move in a direction for a single ant.
	 * \param r_loc the current location
	 * \param direction the direction of the move
	 * \param r_from the maneuver that is being executed
	 * \return bool true if the ant can make a move in that direction, false otherwise.
	 */
	bool MakeMove(const Location& r_loc, int direction, const std::string& r_from = {});

	/**
	* \brief Indicates to the engine that the move is over.
	*/
	void EndTurn();
};

#endif //BOT_H_
