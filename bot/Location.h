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

	// for file debug log
	friend std::ostream& operator <<(std::ostream& r_os, Location const& r_a)
	{
		return r_os << "(" << r_a.row << ';' << r_a.col << ")";
	}
};

inline bool operator<(Location const& r_left, Location const& r_right)
{
	return r_left.row < r_right.row || (r_right.row >= r_left.row && r_left.col < r_right.col); // adapted from std::pair operator<
}

inline bool operator==(Location const& r_left, Location const& r_right)
{
	return r_left.row == r_right.row && r_left.col == r_right.col;
}

inline bool operator!=(Location const& r_left, Location const& r_right)
{
	// the inequality operator is typically implemented in terms of operator==
	// Source: https://en.cppreference.com/w/cpp/language/operators#Comparison_operators
	return !(r_left == r_right);
}

#endif //LOCATION_H_
