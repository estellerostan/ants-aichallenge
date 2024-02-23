#pragma once
#include "State.h"
using namespace std;

class AStar
{
public:

    struct Node {
        Node* parentNode;
        Location position;

        float gCost;
        float hCost;
        float fCost;
    };

    AStar(State& state) : _state(state) {};

    void SetGrid();

    vector<Location> GetPath(Location startLocation, Location destinationLocation);

private:

    float ManhattanDistance(Location currentLocation, Location destinationLocation);
    void ComputeHeuristicCost(Node* currentNode, Node* neighborNode);
    bool IsLocationValid(Location targetLocation);
    bool IsDestination(Location startLocation, Location destinationLocation);
    void BuildPath(vector<Location> &path, Node* currentNode, Location startLocation);
    vector<vector<Node>> _nodeGrid;
    State& _state;
};
