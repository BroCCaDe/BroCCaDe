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
* Entropy_Flow.cc : Implements Entropy_Flow.h                                   *
\*******************************************************************************/

#include <vector>                                       // vector
#include <math.h>                                       // log
#include "Entropy_Flow.h"
#include "Histogram.h"                                  // Histogram

#ifdef DEBUG_H
#include <stdio.h>                                      // printf
#endif

using namespace CCD;

double Entropy::calculate_metric()
{
	unsigned int i;
	Histogram* ptr = static_cast<Histogram*> (_data.get());
//	unsigned int bin_count = ptr->get_bin_count();      // get the number of bins
    double total_data = ptr->get_total_data();
	double entropy = 0.0;
#ifdef DEBUG_H
	printf("Entropy: total data %lu\n", ptr->get_total_data());
#endif
	// iterates over all of the bin
    for (auto it = ptr->begin(); it != ptr->end(); it++)
	{
        double p_i = it->second / total_data;           // p_i
#ifdef DEBUG_H
		printf("%.2f ", p_i);
#endif
		if (p_i != 0.0) 
		  entropy += p_i * log(p_i);                    // sum p_i log(p_i)
	}
#ifdef DEBUG_H
	printf("\n");
#endif
	return -1.0 * entropy;                              // - sum p_i log(p_i)
}
