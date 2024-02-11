#include "AStar.h"

void AStar::SetGrid()
{
    for (int x = 0; x < state.rows; x++)
        for (int y = 0; y < state.cols; y++)
            grid[x][y] = Location(x, y);
}