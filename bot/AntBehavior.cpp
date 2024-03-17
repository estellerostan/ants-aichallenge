#include "AntBehavior.h"

AntBehavior::AntBehavior(Bot* bot)
{
	_bot = bot;
	_pathfinding = new Pathfinding(_bot->state);
}

void AntBehavior::MakeMoves()
{
	//Remove tiles that are not visible
	for (Location loc : _bot->unseenTiles)
	{
		if (_bot->state.grid[loc.row][loc.col].isVisible) _bot->unseenTiles.erase(loc);
		if (_bot->state.grid[loc.row][loc.col].isWater) _bot->unseenTiles.erase(loc);
	}
	//Reset last turn maps
	_bot->orders.clear();
	_bot->targets.clear();

	//Prevent ants from being able to step on their hill
	for (const Location hill : _bot->state.myHills)
	{
		_bot->orders.insert(hill, Location());
	}
		ExploreMap();
		UnblockHills();
}

//Function that uses pathfinding to get an ant to move somewhere I DONT KNOW WHAT I'M DOING HELP
void AntBehavior::MoveToLocation(Location startAnt, Location destinationAnt)
{
	if (find(_bot->state.myAnts.begin(), _bot->state.myAnts.end(), startAnt) != _bot->state.myAnts.end())
	{
		Location nextLocation;

		if (startAnt.row != destinationAnt.row && startAnt.col != destinationAnt.col)
		{
			std::vector<Location> antPath;

			antPath = _pathfinding->GetPath(startAnt, destinationAnt);

			// Remove the first step, which is equal to the current antLoc
			if (!antPath.empty())
			{
				antPath.erase(antPath.begin());

				//insert the new value
				_bot->orders.insert(startAnt, antPath[0]);

				if (_bot->state.isUnoccupied(antPath[0]))
				{
					nextLocation = Location(antPath[0].row, antPath[0].col);
					_bot->makeMove(startAnt, nextLocation);
				}
			}
		}
	}
}

void AntBehavior::ExploreMap()
{
	for (Location antLoc : _bot->state.myAnts)
	{
		const bool hasMove = _bot->containsValue(_bot->orders, antLoc);
		if (!hasMove)
		{
			std::vector<std::tuple<int, Location>> unseenDist;
			for (Location unseenLoc : _bot->unseenTiles)
			{
				auto dist = _bot->state.distance(antLoc, unseenLoc);
				unseenDist.emplace_back(dist, unseenLoc);
				// avoid timeout, even if the search is not complete
				// better than being removed from the game
				if (_bot->state.timeRemaining() < 200) break;
			}
			std::sort(unseenDist.begin(), unseenDist.end());
			for (auto res : unseenDist)
			{
				Location unseenLoc = std::get<1>(res);
				if (_bot->makeMove(antLoc, unseenLoc)) {
					break;
				}
			}
		}
	}
}

void AntBehavior::UnblockHills()
{
	for (Location hillLoc : _bot->state.myHills)
	{
		auto it = std::find(_bot->state.myAnts.cbegin(), _bot->state.myAnts.cend(), hillLoc);
		const bool hasMove = _bot->containsValue(_bot->orders, hillLoc);
		if (it != _bot->state.myAnts.end() && !hasMove)
		{
			// an ant is on a hill
			for (int d = 0; d < TDIRECTIONS; d++)
			{
				if (_bot->makeMove(hillLoc, d))
				{
					break;
				}
			}
		}
	}
}
