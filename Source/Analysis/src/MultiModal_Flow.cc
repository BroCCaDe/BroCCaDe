#include <vector>		// vector
#include <memory>		// shared_ptr
#include "Histogram.h"		// Histogram
#include "MultiModal_Flow.h"	

using namespace CCD;

/*
MultiModal::MultiModal(unsigned int step_size, std::shared_ptr<Bin_Strategy> binner) :
	FlowAnalyzer(std::unique_ptr<Histogram> (new Histogram(binner)), step_size) {}
*/

double MultiModal::calculate_metric()
{
	Histogram* ptr = static_cast<Histogram*> (_data.get());
	unsigned short bin_count = ptr->get_bin_count();
	unsigned long max_i = 0;
	unsigned long sum_square_i = 0;
#ifdef DEBUG
	printf("MultiModal: total data %lu\n", ptr->get_total_data());
#endif
	for (unsigned short i = 0; i < bin_count; i++)
	{
		unsigned long n_i = ptr->get_bin(i);
#ifdef DEBUG
		printf("%.2f ", n_i);
#endif
		sum_square_i = n_i * n_i;				// sum ((n_i)^2)
		if (n_i > max_i) max_i = n_i;				// max (n_i)
	}
#ifdef DEBUG
	printf("\n");
#endif
	return (double) sum_square_i / (double) (max_i * max_i); 	// sum((n_i)^2)/(max(n_i))^2
}
