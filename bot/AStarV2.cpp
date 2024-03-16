#include "AStarV2.h"



std::vector<Location> AStarV2::reconstruct_path(Location start, Location goal, std::map<Location, Location> came_from)
{
    _state.bug << "Building path" << std::endl;
    std::vector<Location> path;
    Location current = goal;
    if (came_from.find(goal) == came_from.end()) {
        return path; // no path can be found
    }
    while (current.row != start.row || current.col != start.col) {
        path.push_back(current);
        current = came_from[current];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());

    _state.bug << "Path is:" << std::endl;
    for (auto path_location : path)
    {
        _state.bug << "Location: " << path_location.row << "," << path_location.col << std::endl;
    }
    return path;
}

void AStarV2::a_star_search(Location start, Location goal, std::map<Location, Location>& came_from, std::map<Location, double>& cost_so_far)
{
    std::vector<std::pair<Location, double>> frontier;
    std::vector<Location> reached;
    frontier.push_back(std::make_pair(start, 0));
    came_from[start] = start;
    cost_so_far[start] = 0;

    while (!frontier.empty()) {
        sort(frontier.begin(), frontier.end(), [=](std::pair<Location, double>& a, std::pair<Location, double>& b)
            {
                return a.second < b.second;
            }
        );

        Location current = frontier.back().first;
        frontier.pop_back();
        reached.push_back(current);
        if (current == goal) {
            _state.bug << "Destination reached" << std::endl;
            break;
        }
        //Compute neighbors as Locations, exclude diagonals and include wrapped map reindexing
        std::vector<Location> neighbors;
        for (int x = -1; x <= 1; x++)
        {
            for (int y = -1; y <= 1; y++)
            {
                if ((abs(x) + abs(y) >= 2))
                {
                    continue;
                }
                else
                {
                    int neighborRow = current.row + x;
                    int neighborCol = current.col + y;
                    //Wrapped map reindexing
                    if (neighborRow >= _state.rows)
                    {
                        neighborRow = 0;
                    }
                    else if (neighborRow < 0)
                    {
                        neighborRow = _state.rows - 1;
                    }
                    if (neighborCol >= _state.cols)
                    {
                        neighborCol = 0;
                    }
                    else if (neighborCol < 0)
                    {
                        neighborCol = _state.cols - 1;
                    }
                    Location newNeighbor = Location(neighborRow, neighborCol);
                    if (IsLocationValid(newNeighbor))
                    {
                        neighbors.push_back(newNeighbor);
                    }
                    else
                    {
                        reached.push_back(newNeighbor);
                        continue;
                    }
                }
            }
        }
        for (Location next : neighbors) {
            _state.bug << "Current Location: " << current.row << "," << current.col << "Neighbor: " << next.row << "," << next.col << std::endl;
            bool isInReachedOrFrontier = false;
            for (Location reachedLoc : reached) 
            {
                if (next == reachedLoc)
                {
                    isInReachedOrFrontier = true;
                }
            }
            for (std::pair<Location, double> frontierLoc : frontier)
            {
                if (next == frontierLoc.first)
                {
                    isInReachedOrFrontier = true;
                }
            }
            
            if (isInReachedOrFrontier == false) 
            {
                double new_cost = cost_so_far[current] + Heuristic(current, next);
                if (cost_so_far.find(next) == cost_so_far.end() || new_cost < cost_so_far[next]) {
                    cost_so_far[next] = new_cost;
                    double priority = new_cost + ManhattanDistance(next, goal);
                    frontier.push_back(std::make_pair(next, priority));
                    came_from[next] = current;
                }
            }

        }
    }
}

std::vector<Location> AStarV2::GetPath(Location startLocation, Location destinationLocation)
{
    _state.bug << "Get path started" << std::endl;
    std::map<Location, Location> came_from;
    std::map<Location, double> cost_so_far;
    a_star_search(startLocation, destinationLocation, came_from, cost_so_far);
    return reconstruct_path(startLocation, destinationLocation, came_from);
}

bool AStarV2::IsLocationValid(Location targetLocation)
{
    return !_state.grid[targetLocation.row][targetLocation.col].isWater;
}

inline double AStarV2::Heuristic(Location a, Location b) {
    float x = std::abs(a.row - b.row);
    float y = std::abs(a.col - b.col);
    if (x > 2)
    {
        x -= _state.rows - 2;
    }
    if (y > 2)
    {
        y -= _state.cols - 2;
    }
    return x + y;
}

inline double AStarV2::ManhattanDistance(Location a, Location b) {
    return std::abs(a.row - b.row) + std::abs(a.col - b.col);
}