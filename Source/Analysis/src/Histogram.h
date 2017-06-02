#ifndef AUX_PLUGINS_HISTOGRAM_H
#define AUX_PLUGINS_HISTOGRAM_H

#include <memory>		// contains unique_ptr and shared_ptr
#include "Data_Container.h"	// the superclass for histogram (Data_Container)
#include "Bin_Strategy.h"	// contains the superclass for the bin allocator

namespace CCD {

// Strip the information from the features and count the bin frequencies
class Histogram : public Data_Container
{
public:
	// Construct a histogram by attaching the bin allocator
	Histogram(std::shared_ptr<Bin_Strategy> binner);
	virtual ~Histogram() {};

	// classify the input feature into a bin and increase the bin's count
	virtual void add_feature(double feature);

	// the average count across all bins
	double get_mean();

	// get the probability of a particular bin
	double get_bin_probability(unsigned short bin_index)
		{return (double)_bins[bin_index] / (double)_total_data;}

	// get the total data across all bins
	unsigned long get_total_data() {return _total_data;}

	// get the count of a particular bin
	unsigned long get_bin(unsigned short bin_index) {return _bins[bin_index];}

	// get the number of bins
	unsigned short get_bin_count() {return _bin_count;}
private:
	std::shared_ptr<Bin_Strategy> _binner;	// bin allocator
	std::unique_ptr<unsigned long[]> _bins; // count for each bin
	unsigned long _total_data;		// total data across all bins
	unsigned short _bin_count;		// the number of bins
};

}

#endif
