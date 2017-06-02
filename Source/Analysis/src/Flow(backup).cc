#include <vector>
#include <math.h>
#include <algorithm>
#include "Flow.h"

Flow_IAT::Flow_IAT(unsigned int window_size, unsigned int step)
{
	_data = new std::vector<double>();
	_data_len = 0;
	_window_size = window_size;
	_step = step;
}

double Flow_IAT::add_IAT(double timestamp) 
{
	_data->push_back(timestamp);
	_data_len++;
	printf("Adding IAT, currently there are %d data\n", _data_len);
	if (_data_len >= _window_size) {
		double res = CalculateMetric();
		ResetWindow();
		return res;
	}
	return -1;
}

void Flow_IAT::ResetWindow()
{
	_data_len = _data_len - _step;
	_data->erase (_data->begin(), _data->begin() + _step);
}

KS_Flow_IAT::KS_Flow_IAT(unsigned int window_size, unsigned int step) :
	Flow_IAT(window_size, step)
{
	_normal_data_len = 0;
	_normal_data = new std::vector<double>();
}

void KS_Flow_IAT::SetNormalData(std::vector<double> normal_data, int n) 
{
	*_normal_data = std::vector<double> (normal_data);
	std::sort (_normal_data->begin(), _normal_data->end());
	_normal_data_len = n;
}

double KS_Flow_IAT::CalculateMetric()
{
	double d;
	double prob;
	printf("\t Calculating Metric \n");
	std::sort (_normal_data->begin(), _normal_data->end());
	kstwo(*_data, _data_len, *_normal_data, _normal_data_len, &d, &prob);
	return d;
}

// modified from numerical recipes by assuming that data1 and data2 are already sorted
void KS_Flow_IAT::kstwo(std::vector<double> data1, unsigned int n1, 
	std::vector<double> data2, unsigned int n2, double *d, double *prob)
{
	printf("\t\t Kolmogorov-Smirnov test\n");
	//float probks(float alam);
	//void sort(unsigned long n, float arr[]);
	unsigned long j1=1,j2=1;
	float d1,d2,dt,en1,en2,en,fn1=0.0,fn2=0.0;

	//sort(n1,data1);
	//sort(n2,data2);
	en1=n1;
	en2=n2;
	*d=0.0;
	while (j1 <= n1 && j2 <= n2) {
		if ((d1=data1[j1]) <= (d2=data2[j2])) fn1=j1++/en1;
		if (d2 <= d1) fn2=j2++/en2;
		if ((dt=fabs(fn2-fn1)) > *d) *d=dt;
	}
	en=sqrt(en1*en2/(en1+en2));
	*prob=probks((en+0.12+0.11/en)*(*d));
}

double KS_Flow_IAT::probks(double alam)
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
