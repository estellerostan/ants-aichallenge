#ifndef LOCATION_H_
#define LOCATION_H_

/*
	struct for representing locations in the grid.
*/
struct Location
{
	int row, col;

	Location()
	{
		row = col = 0;
	}

	Location(int r, int c)
	{
		row = r;
		col = c;
	}
};

inline bool operator<(Location const& left, Location const& right)
{
	return left.row < right.row || (right.row >= left.row && left.col < right.col); // adapted from std::pair operator<
}

#endif //LOCATION_H_
