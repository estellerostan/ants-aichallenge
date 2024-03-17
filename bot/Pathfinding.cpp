#include "Pathfinding.h"

std::vector<Location> Pathfinding::GetPath(Location startLocation, Location destinationLocation)
{
    _state.bug << "Building path" << std::endl;
    std::vector<Location> path;
    std::vector<Location> reached;


    Location current = startLocation;
    while (current.row != destinationLocation.row || current.col != destinationLocation.col) {
        path.push_back(current);
        reached.push_back(current);
            if (current == destinationLocation) {
                _state.bug << "Destination reached" << std::endl;
                break;
            }
            //Compute neighbors as Locations, exclude diagonals and include wrapped map reindexing
            std::vector<std::pair<Location, double>> neighbors;
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
                        if (_state.isUnoccupied(newNeighbor))
                        {
                            neighbors.push_back(std::make_pair(newNeighbor, ManhattanDistance(newNeighbor, destinationLocation)));
                        }
                        else
                        {
                            reached.push_back(newNeighbor);
                            continue;
                        }
                    }
                }
            }
            sort(neighbors.begin(), neighbors.end(), [=](std::pair<Location, double>& a, std::pair<Location, double>& b)
                {
                    return a.second < b.second;
                }
            );
            bool bneighborAdded = false;
            for (auto next : neighbors) {
                if (bneighborAdded == true)
                {
                    reached.push_back(next.first);
                    continue;
                }
                _state.bug << "Current Location: " << current.row << "," << current.col << "Neighbor: " << next.first.row << "," << next.first.col << std::endl;
                bool bisInReached = false;
                
                for (Location reachedLoc : reached)
                {
                    if (next.first == reachedLoc)
                    {
                        bisInReached = true;
                    }
                }           
                if(bisInReached == false)
                {
                    current = next.first;
                    bneighborAdded = true;
                }

        }
    }
    path.push_back(destinationLocation);
    _state.bug << "Path is:" << std::endl;
    for (auto path_location : path)
    {
        _state.bug << "Location: " << path_location.row << "," << path_location.col << std::endl;
    }
    return path;

}

inline double Pathfinding::ManhattanDistance(Location a, Location b) {
    return std::abs(a.row - b.row) + std::abs(a.col - b.col);
}