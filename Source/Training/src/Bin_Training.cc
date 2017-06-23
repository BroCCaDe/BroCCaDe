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

#include "Bin_Training.h"
#include <cfloat>		// DBL_MAX
#include <vector>		// vector
#include <algorithm>		// sort

// the intervals range is [-DBL_MAX, DBL_MAX)
// TODO: currently there will be a problem when the left and right side of 
//   the interval contains the same value due to the nature of the interval
//   (<= x <). In particular, if we have a series of a value v which dominates
//   the data, then we will not be able to accurately divide the data
std::vector< std::pair<double, double> > 
	IntervalTraining::get_equiprobable_interval(unsigned int bin_number)
{
	std::vector< std::pair<double, double> > result;

	// if there is not enough data point, then there are no valid intervals
	if (_data.size() <= bin_number) return result;

	double left = -DBL_MAX;				// lower bound of the interval
	unsigned int step = _data.size() / bin_number;	// the size of each group

	// sort the data series
	std::sort (_data.begin(), _data.end());

	// look for a value to distinguish the group of data series.
	// We first divide the series into groups of equal size according to the bin_number,
	// and then we look at the value on the borders of each group. The intervals
	// are the average value of the borders, e.g.. {l1, ..., r1}, {l2, ..., r2}, {l3, ..., r4}
	// the intervals are {-DBL_MIN, l2 - (l2 - r1) / 2}, {l2 - (l2 - r1) / 2, l3 - (l3 - r2) / 2}, 
	// {l3 - (l3 - r2) / 2, DBL_MAX}
	for (unsigned int i = 1, idx=step; i < bin_number - 1; i++, idx+=step)
	{
		double right = (_data[idx+1] + _data[idx]) / 2.0;
		// simple countermeasure when left = right, add a tiny value
		if (right <= left) right = left + DBL_MIN;
		result.emplace_back(left, right);
		left = right;
	}
	// add the last interval to DBL_MAX
	result.emplace_back(left, DBL_MAX);

	return result;
}
