#ifndef AUX_PLUGINS_BIN_STRATEGY_INTERVAL_H
#define AUX_PLUGINS_BIN_STRATEGY_INTERVAL_H

#include <vector>		// vector
#include "Bin_Strategy.h"	// superclass

namespace CCD {

// TODO: check whether there will be gaps between intervals
// TODO: check overlapping intervals
// description: using interval to determine the bin number of a value.
// currently implemented as a list of intervals to check against (not sorted)
// i.e. the index in the vector is the bin number
class Bin_Strategy_Interval : public Bin_Strategy
{
public:
	Bin_Strategy_Interval() {_intervals.clear();}
	~Bin_Strategy_Interval() {}

	// classify a feature into a bin
	virtual unsigned short get_bin_number(double features);

	// get the number of intervals contained in the list
	virtual unsigned short get_bin_count() {return _intervals.size();}

	// add an interval into the list of intervals
	void add_interval(double min, double max);
private:
	// each interval is represented as a pair of double <min, max>
	std::vector<std::pair<double, double> > _intervals;
};

}

#endif
