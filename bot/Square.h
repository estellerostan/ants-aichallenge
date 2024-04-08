#ifndef SQUARE_H_
#define SQUARE_H_

#include <vector>

/*
    struct for representing a square in the grid.
*/
struct Square
{
    bool isVisible, isWater, isHill, isFood, isMyAnt, isEnemyAnt;
    int ant, hillPlayer;
    std::vector<int> deadAnts;

    Square()
    {
        isVisible = isWater = isHill = isFood = isMyAnt = isEnemyAnt = false;
        ant = hillPlayer = -1;
    }

    //resets the information for the square except water information
    void Reset()
    {
        isVisible = false;
        isHill = false;
        isFood = false;
        isMyAnt = false;
        isEnemyAnt = false;
        ant = hillPlayer = -1;
        deadAnts.clear();
    }
};

enum SquareType
{
    UNKNOWN,
    HILL,
    FOOD,
    MYANT,
    ENEMYANT,
    INVISIBLE
};

#endif //SQUARE_H_
