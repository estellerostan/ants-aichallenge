#include "AStar.h"

void AStar::SetGrid()
{
    for (int x = 0; x < _state.rows; x++)
        for (int y = 0; y < _state.cols; y++)
            _nodeGrid[x][y].position = Location(x, y);
}

vector<Location> AStar::GetPath(Node currentNode, Location destinationNode) {

}

float AStar::ManhattanDistance(Location currentLocation, Location destinationLocation) {
    return abs(currentLocation.row - destinationLocation.row) + abs(currentLocation.col - destinationLocation.col);
}

void AStar::ComputeManhattanCost(Node* currentNode, Node* neighborNode) {
    float manhattanDistance = ManhattanDistance(currentNode->position, neighborNode->position);
    if (currentNode->gCost + manhattanDistance < neighborNode->gCost)
    {
        neighborNode->parentNode = currentNode;
        neighborNode->gCost = currentNode->gCost + manhattanDistance;
        neighborNode->fCost = neighborNode->gCost + neighborNode->hCost;
    }
}