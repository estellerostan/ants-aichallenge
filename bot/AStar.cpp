#include "AStar.h"

using namespace std;

void AStar::SetGrid()
{
    _nodeGrid = std::vector<std::vector<Node>>(_state->rows, vector<Node>(_state->cols, Node()));
    for (int x = 0; x < _state->rows; x++)
        for (int y = 0; y < _state->cols; y++)
            _nodeGrid[x][y].position = Location(x, y);
}

vector<Location> AStar::GetPath(Location startLocation, Location destinationLocation) const {
    vector<Location> shortestPath;
    if (startLocation == destinationLocation) {
        _state->bug << "Destination reached already" << endl;
        return shortestPath;
    }
    if (!IsLocationValid(destinationLocation)) {
        _state->bug << "Destination is not a valid target" << endl;
        return shortestPath;
    }
    //Innit node grid
    vector<vector<Node>> searchGrid = _nodeGrid;
    //H cost for all nodes
    for (int x = 0; x <searchGrid.size(); x++)
        for (int y = 0; y < searchGrid[x].size(); y++)
            searchGrid[x][y].hCost = ManhattanDistance(Location(x, y), destinationLocation);
    //G cost for start node
    searchGrid[startLocation.row][startLocation.col].gCost = 0;
    //F cost for start node
    searchGrid[startLocation.row][startLocation.col].fCost = searchGrid[startLocation.row][startLocation.col].hCost;
    searchGrid[startLocation.row][startLocation.col].parentNode = &searchGrid[startLocation.row][startLocation.col];

    //Queued and explored node lists for A*
    vector<Node*> reachedNodes;
    vector<Node*> queuedNodes;
    //Put start node in the queued node list
    queuedNodes.push_back(&searchGrid[startLocation.row][startLocation.col]);

    //While we still have nodes to explore
    while (!queuedNodes.empty())
    {
        //Sort queuedNodes so that the lowest fcost is last and we look it up first
        sort(queuedNodes.begin(), queuedNodes.end(), [](const Node* node1, const Node* node2) { return node1->fCost > node2->fCost; });
        Node* currentNode = queuedNodes.back();
        queuedNodes.pop_back();
        reachedNodes.push_back(currentNode);

        //If we reached the destination location
        if (currentNode->position == destinationLocation)
        {
            BuildPath(shortestPath, currentNode, startLocation);
        }
        //If not, we explore the neighbor nodes
        //TODO: EXPLORATION OF NEIGHBOR NODES
    }
    return shortestPath;
}

float AStar::ManhattanDistance(Location currentLocation, Location destinationLocation) {
    return static_cast<float>(abs(currentLocation.row - destinationLocation.row) + abs(currentLocation.col - destinationLocation.col));
}

void AStar::ComputeHeuristicCost(Node* currentNode, Node* neighborNode) {
    const auto heuristic = ManhattanDistance(currentNode->position, neighborNode->position);
    if (currentNode->gCost + heuristic < neighborNode->gCost)
    {
        neighborNode->parentNode = currentNode;
        neighborNode->gCost = currentNode->gCost + heuristic;
        neighborNode->fCost = neighborNode->gCost + neighborNode->hCost;
    }
}

bool AStar::IsLocationValid(Location targetLocation) const {
    return !_state->grid[targetLocation.row][targetLocation.col].isWater;
}

void AStar::BuildPath(vector<Location>& path, Node* currentNode, Location startLocation) const {
    path.push_back(currentNode->position);
    while (currentNode->position.row != startLocation.row && currentNode->position.col != startLocation.col)
    {
        currentNode = currentNode->parentNode;
        if (IsLocationValid(currentNode->position)) 
        {
            path.emplace_back(currentNode->position);
        }
        else if(!IsLocationValid(currentNode->position))
        {
            std::vector<int> directions = _state->getDirections(path[path.size() - 1], startLocation);
            const Location temporaryLocation = _state->getLocation(startLocation, directions[0]);
            if (IsLocationValid(temporaryLocation))
            {
                path.push_back(_state->getLocation(path[path.size() - 1], directions[0]));
                path.push_back(_state->getLocation(path[path.size() - 1], directions[0]));
            }
            else
            {
                path.push_back(_state->getLocation(path[path.size() - 1], directions[1]));
                path.push_back(_state->getLocation(path[path.size() - 1], directions[0]));
            }           
        }
    }
    //Start point first
    std::reverse(path.begin(), path.end());
}