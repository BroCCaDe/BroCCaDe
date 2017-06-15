#include <vector>	// vector
#include <math.h>	// exp, fabs, and sqrt
#include <algorithm>	// sort
#include <stdio.h>	// printf
#include <memory>	// shared_ptr and unique_ptr
#include "KS_Flow.h"	

using namespace CCD;

double KS::calculate_metric()
{
	double d;
	double prob;
	Raw_Data* ptr = static_cast<Raw_Data*> (_data.get());
	std::vector<double> contained_data = ptr->get_data();

	// We need to sort the data
	std::sort(contained_data.begin(), contained_data.end());
#ifdef DEBUG
	printf ("KS : total data %d\n", ptr->get_data_len());
#endif
	kstwo(contained_data, ptr->get_data_len(), 
		*_normal_data, _normal_data->size(), &d, &prob);

	// everytime we calculate the metric we reset the data window
	ptr->reset_window();
	return d;
}

// modified from numerical recipes by assuming that data1 and data2 are already sorted
void KS::kstwo(std::vector<double> data1, std::vector<double> data2, double *d, double *prob)
{
	unsigned long j1=0,j2=0;
	unsigned int n1, n2;
	double d1,d2,dt,en1,en2,en,fn1=0.0,fn2=0.0;

	n1 = data1.size();
	n2 = data2.size();
	en1=n1;
	en2=n2;
	*d=0.0;
	while (j1 < n1 && j2 < n2) {
		if ((d1=data1[j1]) <= (d2=data2[j2])) fn1=j1++/en1;
		if (d2 <= d1) fn2=j2++/en2;
		if ((dt=fabs(fn2-fn1)) > *d) *d=dt;
	}
	en=sqrt(en1*en2/(en1+en2));
	*prob=probks((en+0.12+0.11/en)*(*d));
}

double KS::probks(double alam)
{
	int j;
	float a2,fac=2.0,sum=0.0,term,termbf=0.0;
	a2 = -2.0*alam*alam;
	for (j=1;j<=100;j++) {
		term=fac*exp(a2*j*j);
		sum += term;
		if (fabs(term) <= EPS1*termbf || fabs(term) <= EPS2*sum) return sum;
		fac = -fac; //Alternating signs in sum
		termbf=fabs(term);
	}
	return 1.0; //Get here only by failing to converge
}
