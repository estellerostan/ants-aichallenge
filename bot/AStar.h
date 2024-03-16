#pragma once
#include "State.h"

class AStar
{
public:

    struct Node {
        Node* parentNode;
        Location position;

        float gCost;
        float hCost;
        float fCost;

        // for file debug log
        friend std::ostream& operator <<(std::ostream& os, Node const& a)
        {
            return os << "Node: " << a.position << std::endl
                << "Cost (g,h,f): " << a.gCost << ", " << a.hCost << ", " << a.fCost << std::endl << std::endl;
        }
    };

    explicit AStar(State& state) : _state(state) {}

    void SetGrid();

    std::vector<Location> GetPath(Location startLocation, Location destinationLocation);

private:

    float ManhattanDistance(Location currentLocation, Location destinationLocation);
    void ComputeHeuristicCost(Node* currentNode, Node* neighborNode);
    bool IsLocationValid(Location targetLocation) const;
    void BuildPath(std::vector<Location>& path, Node* currentNode, Location startLocation) const;
    std::vector<std::vector<Node>> _nodeGrid;
    State& _state;
    
};