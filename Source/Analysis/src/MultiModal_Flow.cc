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
\*******************************************************************************/

#include <vector>		// vector
#include <memory>		// shared_ptr
#include "Histogram.h"		// Histogram
#include "MultiModal_Flow.h"	

using namespace CCD;

double MultiModal::calculate_metric()
{
	Histogram* ptr = static_cast<Histogram*> (_data.get());
	unsigned short bin_count = ptr->get_bin_count();		// get the number of bins
	unsigned long max_i = 0;
	unsigned long sum_square_i = 0;
#ifdef DEBUG
	printf("MultiModal: total data %lu\n", ptr->get_total_data());
#endif
	for (unsigned short i = 0; i < bin_count; i++)
	{
		unsigned long n_i = ptr->get_bin(i);
#ifdef DEBUG
		printf("%u ", n_i);
#endif
		sum_square_i += n_i * n_i;				// sum ((n_i)^2)
		if (n_i > max_i) max_i = n_i;				// max (n_i)
	}
#ifdef DEBUG
	printf("\n");
#endif
	return (double) sum_square_i / (double) (max_i * max_i); 	// sum((n_i)^2)/(max(n_i))^2
}
