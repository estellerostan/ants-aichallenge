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
	friend std::ostream& operator <<(std::ostream& r_os, Ant const& r_a)
	{
		return r_os << "Ant: " << r_a.loc << std::endl
			<< "Stats:" << r_a.nbAttackers << " attackers, dead: " << r_a.isDead << std::endl
			<< "Dest: " << r_a.dest << ", " << r_a.bestDest << std::endl << std::endl;
	}
};

