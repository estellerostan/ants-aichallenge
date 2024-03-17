#pragma once
#include "State.h"

/**
* \brief Class AStar: contains a version (V1) of an A* algorithm and its associated function that computes paths between two locations.
*
*/
class AStar
{
public:

    //Structure for a graph node used in A* algorithms.
    struct Node {
        Node* parentNode;
        Location position;

        float gCost;
        float hCost;
        float fCost;

        //For file debug log.
        friend std::ostream& operator <<(std::ostream& os, Node const& a)
        {
            return os << "Node: " << a.position << std::endl
                << "Cost (g,h,f): " << a.gCost << ", " << a.hCost << ", " << a.fCost << std::endl << std::endl;
        }
    };

    /**
     * \brief Constructor for the AStar class.
     * \param State& state reference to the current state of the game.
     */
    explicit AStar(State& state) : _state(state) {}

    /**
     * \brief Initialize a node grid that represents the map.
     */
    void SetGrid();

    /**
     * \brief Computes a path between two locations.
     * \param Location startLocation the starting point of the path.
     * \param Location destinationLocation the destination point of the path.
     * \return vector<Location>, containing the location points forming a path between startLocation and destinationLocation.
     */
    std::vector<Location> GetPath(Location startLocation, Location destinationLocation);

private:

    /**
     * \brief Computes the Manhattan distance between two locations.
     * \param Location a first location.
     * \param Location b second location.
     * \return Float, the Manhattan distance value between a and b.
     */
    float ManhattanDistance(Location currentLocation, Location destinationLocation);

    /**
     * \brief Computes the A* heuristic using the Manhattan distance between the location of two nodes on the grid, updates the neighbor node costs.  Takes into consideration the wrapped aspect of the map
     * \param Node* currentNode pointer towards the currently analyzed node.
     * \param Node* neighborNode pointer towards a neighbor node of the current node.
     */
    void ComputeHeuristicCost(Node* currentNode, Node* neighborNode);

    /**
     * \brief Checks if the Location given in parameter is a valid location (not a water tile).
     * \param Location targetLocation location to check.
     * \return Bool true if the location is valid, false otherwise.
     */
    bool IsLocationValid(Location targetLocation) const;

    /**
     * \brief Builds a path between two nodes.
     * \param vector<Location>& path vector that will contain the path to build.
     * \param Node* currentNode a node.
     * \param Location startLocation the starting point of the path.
     */
    void BuildPath(std::vector<Location>& path, Node* currentNode, Location startLocation) const;

    //Grid of nodes that represents the map.
    std::vector<std::vector<Node>> _nodeGrid;

    //Reference & to the current state of the game.
    State& _state;
    
};