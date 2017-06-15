#include <vector>		// vector
#include <math.h>		// log
#include "Entropy_Flow.h"
#include "Histogram.h"		// Histogram

#ifdef DEBUG_H
#include <stdio.h>		// printf
#endif

using namespace CCD;

double Entropy::calculate_metric()
{
	unsigned short i;
	Histogram* ptr = static_cast<Histogram*> (_data.get());
	unsigned short bin_count = ptr->get_bin_count();	// get the number of bins
	double entropy = 0.0;
#ifdef DEBUG_H
	printf("Entropy: total data %lu\n", ptr->get_total_data());
#endif
	// iterates over all of the bin
	for (i = 0; i < bin_count; i++)
	{
		double p_i = ptr->get_bin_probability(i);	// p_i
#ifdef DEBUG_H
		printf("%.2f ", p_i);
#endif
		if (p_i != 0.0) 
		  entropy += p_i * log(p_i); 			// sum p_i log(p_i)
	}
#ifdef DEBUG_H
	printf("\n");
#endif
	return -1.0 * entropy; 					// - sum p_i log(p_i)
}
