#pragma once
#include "Location.h"

class Ant
{
public:
	Ant() {}
	Ant(Location loc) : loc(loc){}
	Location loc = Location(-1, -1);
	Location dest = Location(-1, -1);
	Location bestDest = Location(-1, -1);
	int nbAttackers = 0;
	bool isDead = false;

	// for file debug log
	friend std::ostream& operator <<(std::ostream& os, Ant const& a)
	{
		return os << "Ant: " << a.loc << std::endl
			<< "Stats:" << a.nbAttackers << " attackers, dead: " << a.isDead << std::endl
			<< "Dest: " << a.dest << ", " << a.bestDest << std::endl << std::endl;
	}
};

