#include <vector>			// vector
#include <math.h>			// abs and sqrt
#include "Regularity_Flow.h"		// superclass
#include "Regularity_Data_Container.h"	// data container for the standard deviations

#ifdef DEBUG_H
#include <stdio.h>			// printf
#endif

using namespace CCD;

double Regularity::calculate_metric()
{
	unsigned short i, j;
	int N;
	double sum = 0.0;		// sum (|sigma_j - sigma_i| / sigma_i)
	double sum_square = 0.0;	// sum ((|sigma_j - sigma_i| / sigma_j)^2)
	Regularity_Data* ptr = static_cast<Regularity_Data*> (_data.get());
	
	std::vector<double> stdev (ptr->get_stdev());
	N = stdev.size();

	for (j = 0; j < stdev.size(); j++) // forall j
	{
		for (i = 0; i < j; i++) // forall i < j
		{
			// |sigma_j - sigma_i| / sigma_i
			double r = abs(stdev[j] - stdev[i]) / stdev[i];
			sum += r;
			sum_square += r * r;
		}
	}
	
	// stdev (|sigma_j - sigma_i| / sigma_i) for all j, i < j
	return sqrt((N * sum_square) - (sum * sum)) / N;
}
