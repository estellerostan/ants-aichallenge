#include "AStar.h"

void AStar::SetGrid()
{
    for (int x = 0; x < _state.rows; x++)
        for (int y = 0; y < _state.cols; y++)
            _nodeGrid[x][y].position = Location(x, y);
}

vector<Location> AStar::GetPath(Node currentNode, Location destinationNode) {

}

double AStar::ComputeManhattanCost(Node* currentNode, Node* neighborNode) {

}