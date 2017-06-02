#include "Bin_Strategy_Interval.h"
#include <vector>			// vector
#include <iostream>			// cout

using namespace CCD;

unsigned short Bin_Strategy_Interval::get_bin_number(double feature)
{
	unsigned int i = 0;
	// check which interval fits the feature
	for (i = 0; i < _intervals.size(); i++)
	{
		if (feature >= _intervals[i].first && 
				feature < _intervals[i].second)
		{
			return i;
		}
	}
	// this should never happen, so the return value is arbitrary
	std::cout << "weird feature: " << feature << "\n";
	return i;	
}

void Bin_Strategy_Interval::add_interval(double min, double max)
{
	_intervals.push_back(std::make_pair(min, max));
}
