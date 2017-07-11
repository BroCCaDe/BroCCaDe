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
* Bin_Training.h : holds the data series and print it out both in the raw format*
*       and the intervals for equiprobable bin. Raw format is useful for KS     *
*       normal data while the intervals are useful for the binning strategy,    *
*       e.g. for Histogram and CCE pattern tree                                 *
\*******************************************************************************/

#ifndef AUX_PLUGIN_BIN_TRAINING_H
#define AUX_PLUGIN_BIN_TRAINING_H

#include <vector>	// vector
#include <memory>

// The class to get the intervals for the equiprobable bins and
// the "normal" data series for the Kolmogorov-Smirnov test
class IntervalTraining
{
public: 
	IntervalTraining() {_data.clear();}
	~IntervalTraining() {}

	// add a data point
	void add_feature(double feature) {_data.push_back(feature);}

	// get equiprobable bins based on the data series
	std::vector< std::pair<double, double> > get_equiprobable_interval(unsigned int bin_number);

	// get the whole data series
	std::vector<double> get_data() {return _data;}

	// get the size of the data
	unsigned int get_data_length() {return _data.size();}
private:
	std::vector<double> _data;	// data series
};

class BiFlow
{
public:
    std::shared_ptr<IntervalTraining> get(int direction, int tag);
private:
    std::vector<std::vector<std::shared_ptr<IntervalTraining> > > _flows;
};

#endif
