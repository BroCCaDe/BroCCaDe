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
* Bin_Strategy_Interval.h : inherits class Bin_Strategy. A bin allocator which  *
*       determines the bin number based on an closed-open interval [).          *
\*******************************************************************************/

#ifndef AUX_PLUGINS_BIN_STRATEGY_INTERVAL_H
#define AUX_PLUGINS_BIN_STRATEGY_INTERVAL_H

#include <vector>           // vector
#include "Bin_Strategy.h"   // superclass

namespace CCD {

// TODO: check whether there will be gaps between intervals
// TODO: check overlapping intervals
// description: using interval to determine the bin number of a value.
// currently implemented as a list of intervals to check against (not sorted)
// i.e. the index in the vector is the bin number
class Bin_Strategy_Interval : public Bin_Strategy
{
public:
	Bin_Strategy_Interval() {_intervals.clear();}
	~Bin_Strategy_Interval() {}

	// classify a feature into a bin
	virtual unsigned short get_bin_number(double features);

	// get the number of intervals contained in the list
	virtual unsigned int get_bin_count() {return _intervals.size();}

	// add an interval into the list of intervals
	void add_interval(double min, double max);
private:
	// each interval is represented as a pair of double <min, max>
	std::vector<std::pair<double, double> > _intervals;
};

}

#endif
