#include "FoodHunt.h"

//Previously named GatherFood()
void FoodHunt::MakeMoves()
{
	std::vector<std::tuple<double, Location, Location>> antDist;
	// find close food
	for (auto foodLoc : _bot->state.food)
	{
		for (Location antLoc : _bot->state.myAnts)
		{
			auto dist = _bot->state.distance(antLoc, foodLoc);

			antDist.emplace_back(dist, antLoc, foodLoc);
		}
	}

	std::sort(antDist.begin(), antDist.end());

	for (auto res : antDist)
	{
		Location antLoc = std::get<1>(res);
		Location foodLoc = std::get<2>(res);

		// if food has no ant gathering it and ant has no task
		const bool isAntBusyWithFood = _bot->containsValue(_bot->targets, antLoc);
		if (_bot->targets.count(foodLoc) == 0 && !isAntBusyWithFood) {
			_bot->makeMove(antLoc, foodLoc);
		}
	}
}