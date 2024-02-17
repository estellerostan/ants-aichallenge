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

    vector<Location> GetPath(Node currentNode, Location destinationNode);

private:

    double ComputeManhattanCost(Node* currentNode, Node* neighborNode);
    vector<vector<Node>> _nodeGrid;
    State& _state;
};
