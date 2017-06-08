#include "Histogram.h"
#include <string.h>	// contains memset
#include <stdio.h>	// for printing (printf)
#include <memory>	// contains shared_ptr and unique_ptr
#include <iostream>	// for printing (cout)

using namespace CCD;

// Constructor : takes an input an instance of bin allocator.
// Reset all the data according to the information contained in the bin allocator
Histogram::Histogram(std::shared_ptr<Bin_Strategy> binner)
{
	_binner = binner;
	_total_data = 0;
	_bin_count = binner->get_bin_count();
	_bins = std::unique_ptr<unsigned long[]> (new unsigned long[_bin_count]);
	memset(_bins.get(), 0, sizeof(unsigned long) * _bin_count);
}

void Histogram::add_feature(double feature)
{
//	printf("Histogram::add_feature got here\n");
	// classify the input feature into a bin
	unsigned short bin_number = _binner->get_bin_number(feature);
	// if the bin number is outside of the boundary, print an error and return
	if (bin_number >= _bin_count)
	{
		std::cout << "weird bin number " << bin_number << "(" << _bin_count << ")\n";
		return;
	}

	_bins[bin_number]++;	// increase the count for the bin associated
	_total_data++;		// increase the total data count
#ifdef DEBUG
	printf("HISTOGRAM feature : %lf, bin : %d, total_data : %lu\n", 
		feature, bin_number, _total_data);
#endif
}

double Histogram::get_mean()
{
	unsigned long dominator = 0;
	unsigned short i;
	for (i = 0; i < _bin_count; i++) dominator += _bins[i];
	return dominator / _bin_count;
}
