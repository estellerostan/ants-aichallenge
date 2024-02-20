#include "AStar.h"

void AStar::SetGrid()
{
    for (int x = 0; x < _state.rows; x++)
        for (int y = 0; y < _state.cols; y++)
            _nodeGrid[x][y].position = Location(x, y);
}

vector<Location> AStar::GetPath(Node currentNode, Location destinationNode) {

}

double AStar::ManhattanDistance(Node* currentNode, Node* neighborNode) {
    return abs(currentNode->position.row - neighborNode->position.row) + abs(currentNode->position.col - neighborNode->position.col);
}

void AStar::ComputeManhattanCost(Node* currentNode, Node* neighborNode) {
    int manhattanDistance = ManhattanDistance(currentNode, neighborNode);
    if (currentNode->gCost + manhattanDistance < neighborNode->gCost)
    {
        neighborNode->parentNode = currentNode;
        neighborNode->gCost = currentNode->gCost + manhattanDistance;
        neighborNode->fCost = neighborNode->gCost + neighborNode->hCost;
    }
}