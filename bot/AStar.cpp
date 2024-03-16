#include "AStar.h"
using namespace std;

void AStar::SetGrid()
{
    _nodeGrid = std::vector<std::vector<Node>>(_state.rows, vector<Node>(_state.cols, Node()));
    for (int x = 0; x < _state.rows; x++)
    {
        for (int y = 0; y < _state.cols; y++)
        {
            _nodeGrid[x][y].position = Location(x, y);
        }
            
    }       
}

vector<Location> AStar::GetPath(Location startLocation, Location destinationLocation){
    vector<Location> shortestPath;
    if (startLocation == destinationLocation) {
        _state.bug << "Destination reached already" << endl;
        return shortestPath;
    }
    if (!IsLocationValid(destinationLocation)) {
        _state.bug << "Destination is not a valid target" << endl;
        return shortestPath;
    }
    //Innit node grid
    vector<vector<Node>> searchGrid = _nodeGrid;
    //H cost for all nodes
    for (int x = 0; x < searchGrid.size(); x++)
    {
        for (int y = 0; y < searchGrid[x].size(); y++)
        {
            searchGrid[x][y].hCost = ManhattanDistance(Location(x, y), destinationLocation);
            searchGrid[x][y].gCost = 10;
        }
    }
        
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

        //Debug
        _state.bug << "start " << *currentNode;

        //If we reached the destination location
        if (currentNode->position == destinationLocation)
        {
            _state.bug << "FOUND SHORTEST PATH" << endl;
            BuildPath(shortestPath, currentNode, startLocation);
        }
        //If not, we explore the neighbor nodes
        //TODO: EXPLORATION OF NEIGHBOR NODES
        for (int x = -1; x <= 1; x++) 
        {
            for (int y = -1; y <= 1; y++)
            {
                int neighborNodeRow = currentNode->position.row + x;
                int neighborNodeCol = currentNode->position.col + y;
                //Wrapped map reindexing
                if (neighborNodeRow >= _state.rows)
                {
                    neighborNodeRow = 0;
                }
                else if (neighborNodeRow < 0)
                {
                    neighborNodeRow = _state.rows - 1;
                }
                if (neighborNodeCol >= _state.cols)
                {
                    neighborNodeCol = 0;
                }
                else if (neighborNodeCol < 0)
                {
                    neighborNodeCol = _state.cols - 1;
                }

                Node* neighborNode = &searchGrid[neighborNodeRow][neighborNodeCol];

                if ((abs(x) + abs(y) >= 2)) 
                {
                    reachedNodes.push_back(neighborNode);
                    continue;
                }
                //if(neighborNode->gCost == 0 && find(reachedNodes.begin(), reachedNodes.end(), neighborNode) == reachedNodes.end() && find(queuedNodes.begin(), queuedNodes.end(), neighborNode) == queuedNodes.end()) neighborNode->gCost = 50;
                _state.bug << "neighbor " << *neighborNode;


                //If the neighbor node is not a water tile and not a diagonale, we analyse it
                if (IsLocationValid(neighborNode->position))
                {
                    // If the neighbor node has not been reached and is not in queue for analysis yet
                    if (find(reachedNodes.begin(), reachedNodes.end(), neighborNode) == reachedNodes.end() && find(queuedNodes.begin(), queuedNodes.end(), neighborNode) == queuedNodes.end())
                    {
                        //We save its previous cost and compute the heuristic cost for this path
                        float const previousCost = neighborNode->gCost;
                        ComputeHeuristicCost(currentNode, neighborNode);
                        _state.bug << "gCost " << neighborNode->gCost << " vs " << previousCost << endl;
                        if (neighborNode->gCost < previousCost)
                        {
                            _state.bug << "QUEUE BEING FILLED" << *neighborNode;
                            const auto iterator = find(queuedNodes.begin(), queuedNodes.end(), neighborNode);
                            if (iterator != queuedNodes.end())
                            {
                                queuedNodes.erase(iterator);
                            }
                            queuedNodes.push_back(neighborNode);
                        }
                    }
                }
                //If the neighbor node cannot be analyzed
                else
                {
                    reachedNodes.push_back(neighborNode);
                }
            }
        }
        //Debug
        /*for (auto reached_node : reachedNodes)
        {
            _state.bug << "reached " << *reached_node << endl;
        }
        for (auto queued_node : queuedNodes)
        {
            _state.bug << "queued " << *queued_node << endl;
        }*/
        //END: EXPLORATION OF NEIGHBOR NODES
    }
    _state.bug << "Shortest path:" << endl;
    for (auto path_node : shortestPath) 
    {
        _state.bug << "Location: " << path_node.row << "," << path_node.col<< endl;
    }
    return shortestPath;
}

float AStar::ManhattanDistance(Location currentLocation, Location destinationLocation) {
    return abs(currentLocation.row - destinationLocation.row) + abs(currentLocation.col - destinationLocation.col);
}

void AStar::ComputeHeuristicCost(Node* currentNode, Node* neighborNode){
    float x = abs(currentNode->position.row - neighborNode->position.row);
    float y = abs(currentNode->position.col - neighborNode->position.col);
    if (x > 2)
    {
        x -= _state.rows - 2;
    }
    if (y > 2)
    {
        y -= _state.cols - 2;
    }
    float heuristic =  x + y;
    if (currentNode->gCost + heuristic < neighborNode->gCost || neighborNode->gCost == 0)
    {
        neighborNode->parentNode = currentNode;
        neighborNode->gCost = currentNode->gCost + heuristic;
        neighborNode->fCost = neighborNode->gCost + neighborNode->hCost;
        _state.bug << "New neighbor node values:" << "Cost (g,h,f): " << neighborNode->gCost << ", " << neighborNode->hCost << ", " << neighborNode->fCost << endl;
    }
}

bool AStar::IsLocationValid(Location targetLocation) const {
    return !_state.grid[targetLocation.row][targetLocation.col].isWater;
}

void AStar::BuildPath(vector<Location>& path, Node* currentNode, Location startLocation) const {
    path.push_back(currentNode->position);
    while (currentNode->position.row != startLocation.row && currentNode->position.col != startLocation.col)
    {
        _state.bug << "queue1" << currentNode << endl;
        currentNode = currentNode->parentNode;
        _state.bug << "queue2" << currentNode << endl;

        if (IsLocationValid(currentNode->position))
        {
            path.emplace_back(currentNode->position);
        }
        else if (!IsLocationValid(currentNode->position))
        {
            std::vector<int> directions = _state.getDirections(path[path.size() - 1], startLocation);
            const Location temporaryLocation = _state.getLocation(startLocation, directions[0]);
            if (IsLocationValid(temporaryLocation))
            {
                path.push_back(_state.getLocation(path[path.size() - 1], directions[0]));
                path.push_back(_state.getLocation(path[path.size() - 1], directions[0]));
            }
            else
            {
                path.push_back(_state.getLocation(path[path.size() - 1], directions[1]));
                path.push_back(_state.getLocation(path[path.size() - 1], directions[0]));
            }
        }
    }
	//Start point first
    std::reverse(path.begin(), path.end());
}