/*******************************************************************************\
* Copyright (c) 2017 by Hendra Gunadi (Hendra.Gunadi@murdoch.edu.au)            *
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
* Regularity_Flow.cc : Implements Regularity_Flow.h                             *
\*******************************************************************************/

#include <vector>                       // vector
#include <math.h>                       // abs and sqrt
#include <cmath>
#include "Regularity_Flow.h"            // superclass
#include "Regularity_Data_Container.h"  // data container for the standard deviations

#ifdef DEBUG_H
#include <stdio.h>                      // printf
#endif

using namespace CCD;

double Regularity::calculate_metric()
{
	unsigned short i, j, n;
	double N;
	double sum = 0.0;		            // sum (|sigma_j - sigma_i| / sigma_i)
	double sum_square = 0.0;	        // sum ((|sigma_j - sigma_i| / sigma_j)^2)
    double d;
	Regularity_Data* ptr = static_cast<Regularity_Data*> (_data.get());
	
	std::vector<double> stdev (ptr->get_stdev());
    n = stdev.size();
    if (n == 0) return 0.0;
	N = (double) n * ((double) n - 1.0) / 2.0;
    if (N == 0.0) N = 1.0;              // only happen when there is only 1 data point

	for (j = 0; j < n; j++) // forall j
	{
		for (i = 0; i < j; i++) // forall i < j
		{
            if (stdev[i] == 0.0) continue;
			// |sigma_j - sigma_i| / sigma_i
			double r = std::abs(stdev[j] - stdev[i]) / stdev[i];
			sum += r;
			sum_square += r * r;
		}
	}
	
	// stdev (|sigma_j - sigma_i| / sigma_i) for all j, i < j
    d = (N * sum_square) - (sum * sum);
    if (d < 0.0) return 0.0;
	return sqrt(d) / N;
}
