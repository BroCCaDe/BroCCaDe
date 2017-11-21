/*******************************************************************************\
* Copyright (c) 2017 Murdoch University                                         *
*                                                                               *
* Written by Hendra Gunadi (Hendra.Gunadi@murdoch.edu.au)                       *
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
* Bin_Strategy_Interval.cc : Implements Bin_Strategy_Interval.h                 *
\*******************************************************************************/

#include "Bin_Strategy_Interval.h"
#include <vector>                   // vector
#include <iostream>                 // cout
#include <algorithm>

#define EPS 1E-3

using namespace CCD;

int test_value_in_interval(double value, std::pair<double, double> interval)
{
    if (value >= interval.first && value < interval.second) return 0;
    if (value >= interval.second) return 1;
    return -1;
}

// sequential search
unsigned int Bin_Strategy_Interval::search_in_interval(double value)
{
    for (unsigned int i = 0; i < _intervals.size(); i++)
        if (test_value_in_interval(value, _intervals[i]) == 0) return i;
    return _intervals.size();
}

// binary search : require that the intervals are sorted in ascending order
unsigned int Bin_Strategy_Interval::binary_search_in_interval(double value)
{
    unsigned int left = 0;
    unsigned int right = _intervals.size()-1;
    while (left <= right)
    {
        unsigned int mid = (left + right) / 2;
        int test = test_value_in_interval(value, _intervals[mid]);
        if (test == 0) return mid;
        else if (test > 0) left = mid + 1;
        else right = mid - 1;
    }
    // this should never happen, so the return value is arbitrary
	std::cout << "weird feature: " << value << "\n";
    return _intervals.size();
}

unsigned short Bin_Strategy_Interval::get_bin_number(double feature)
{
	return binary_search_in_interval(feature);;
    //return search_in_interval(feature);
}

bool interval_comp(std::pair<double, double> i, std::pair<double, double> j) {return (i.first < j.first);}

void Bin_Strategy_Interval::sort_intervals()
{
    std::sort(_intervals.begin(), _intervals.end(), interval_comp);
}

bool Bin_Strategy_Interval::sanity_check()
{
    ssize_t N = _intervals.size();
    for (ssize_t i = 0; i < N - 1; i++)
    {
        // if there is a gap, or if there is an overlap
        if (_intervals[i+1].first - _intervals[i].second > EPS ||
            _intervals[i+1].first - _intervals[i].second < -EPS) return false;
    }
    return true;
}

void Bin_Strategy_Interval::add_interval(double min, double max)
{
	_intervals.push_back(std::make_pair(min, max));
}
