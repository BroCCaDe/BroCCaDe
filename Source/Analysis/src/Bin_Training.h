#ifndef AUX_PLUGIN_BIN_TRAINING_H
#define AUX_PLUGIN_BIN_TRAINING_H

#include <vector>	// vector

// The class to get the intervals for the equiprobable bins and
// the "normal" data series for the Kolmogorov-Smirnov test
class IntervalTraining
{
public: 
	IntervalTraining() {_data.clear();}
	~IntervalTraining() {}

	// add a data point
	void add_feature(double feature) {_data.push_back(feature);}

	// get equiprobable bins based on the data series
	std::vector< std::pair<double, double> > get_equiprobable_interval(unsigned int bin_number);

	// get the whole data series
	std::vector<double> get_data() {return _data;}

	// get the size of the data
	unsigned int get_data_length() {return _data.size();}
private:
	std::vector<double> _data;	// data series
};

#endif
