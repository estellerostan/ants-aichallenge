#pragma once
#include "State.h"
using namespace std;

class AStar
{
public:

    vector<vector<Location>> grid;

    State& state;

    AStar(State& state) : state(state) {};

    void SetGrid();

    vector<Location> GetPath(Node currentNode, Location destinationNode);

private:

    double ComputeEuchlidianCost(Node* currentNode, Node* neighborNode);
    double ComputeManhattanCost(Node* currentNode, Node* neighborNode);

    void UpdateNeighbors(Node* CurrentNode, Node* NeighbourNode, vector<Node*>& availableNodes);
};
//Make a Node class with info relative to the cost, the location, the next Node etc
//Check class material from L2