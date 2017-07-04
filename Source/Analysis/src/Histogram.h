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
* Histogram.h : Histogram data container. Require a binning strategy to         *
*       determine the bin for each incoming feature value.                      *
\*******************************************************************************/

#ifndef AUX_PLUGINS_HISTOGRAM_H
#define AUX_PLUGINS_HISTOGRAM_H

#include <memory>		// contains unique_ptr and shared_ptr
#include "Data_Container.h"	// the superclass for histogram (Data_Container)
#include "Bin_Strategy.h"	// contains the superclass for the bin allocator

namespace CCD {

// Strip the information from the features and count the bin frequencies
class Histogram : public Data_Container
{
public:
	// Construct a histogram by attaching the bin allocator
	Histogram(std::shared_ptr<Bin_Strategy> binner);
	virtual ~Histogram() {};

	// classify the input feature into a bin and increase the bin's count
	virtual void add_feature(double feature);

	// the average count across all bins
	double get_mean();

	// get the probability of a particular bin
	double get_bin_probability(unsigned short bin_index)
		{return (double)_bins[bin_index] / (double)_total_data;}

	// get the total data across all bins
	unsigned long get_total_data() {return _total_data;}

	// get the count of a particular bin
	unsigned long get_bin(unsigned short bin_index) {return _bins[bin_index];}

	// get the number of bins
	unsigned short get_bin_count() {return _bin_count;}
private:
	std::shared_ptr<Bin_Strategy> _binner;	// bin allocator
	std::unique_ptr<unsigned long[]> _bins; // count for each bin
	unsigned long _total_data;		// total data across all bins
	unsigned short _bin_count;		// the number of bins
};

}

#endif
