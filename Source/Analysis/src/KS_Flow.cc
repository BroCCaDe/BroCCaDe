/*******************************************************************************\
* Copyright (c) 2017 by Hendra Gunadi (Hendra.Gunadi@murodch.edu.au)            *
*                                                                               *
* Redistribution and use in source and binary forms, with or without            *
* modification, are permitted provided that the following conditions are met:   *
*                                                                               *
* (1) Redistributions of source code must retain the above copyright            *
*     notice, this list of conditions and the following disclaimer.             *
*                                                                               *
* (2) Redistributions in binary form must reproduce the above copyright         *
*     notice, this list of conditions and the following disclaimer in           *
*     the documentation and/or other materials provided with the                *
*     distribution.                                                             *
*                                                                               *
* (3) Neither the name of Hendra Gunadi and/or Murdoch University, nor          *
*     the names of contributors may be used to endorse or promote               *
*     products derived from this software without specific prior written        *
*     permission.                                                               *
*                                                                               *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"   *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE     *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE    *
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF          *
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS      *
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN       *
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)       *
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE    *
* POSSIBILITY OF SUCH DAMAGE.                                                   *
*                                                                               *
* KS_Flow.cc : Implements KS_Flow.h                                                   *
\*******************************************************************************/

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
	kstwo(contained_data, *_normal_data, &d, &prob);

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
