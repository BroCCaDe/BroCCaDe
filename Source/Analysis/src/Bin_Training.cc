#include "Bin_Training.h"
#include <cfloat>		// DBL_MAX
#include <vector>		// vector
#include <algorithm>		// sort

// the intervals range is [-DBL_MAX, DBL_MAX)
// TODO: currently there will be a problem when the left and right side of 
//   the interval contains the same value due to the nature of the interval
//   (<= x <). In particular, if we have a series of a value v which dominates
//   the data, then we will not be able to accurately divide the data
std::vector< std::pair<double, double> > 
	IntervalTraining::get_equiprobable_interval(unsigned int bin_number)
{
	std::vector< std::pair<double, double> > result;

	// if there is not enough data point, then there are no valid intervals
	if (_data.size() <= bin_number) return result;

	double left = -DBL_MAX;				// lower bound of the interval
	unsigned int step = _data.size() / bin_number;	// the size of each group

	// sort the data series
	std::sort (_data.begin(), _data.end());

	// look for a value to distinguish the group of data series.
	// We first divide the series into groups of equal size according to the bin_number,
	// and then we look at the value on the borders of each group. The intervals
	// are the average value of the borders, e.g.. {l1, ..., r1}, {l2, ..., r2}, {l3, ..., r4}
	// the intervals are {-DBL_MIN, l2 - (l2 - r1) / 2}, {l2 - (l2 - r1) / 2, l3 - (l3 - r2) / 2}, 
	// {l3 - (l3 - r2) / 2, DBL_MAX}
	for (unsigned int i = 1, idx=step; i < bin_number - 1; i++, idx+=step)
	{
		double right = _data[idx+1] - (_data[idx+1] - _data[idx]) / 2.0;
		// simple countermeasure when left = right, add a tiny value
		if (right == left) right += DBL_MIN
		result.emplace_back(left, right);
		left = right;
	}
	// add the last interval to DBL_MAX
	result.emplace_back(left, DBL_MAX);

	return result;
}
