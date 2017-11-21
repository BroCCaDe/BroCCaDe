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
* Regularity_Data_Container.cc : Implements Regularity_Data_Container.h         *
\*******************************************************************************/

#include "Regularity_Data_Container.h"
#include <math.h>                       // sqrt

#ifdef DEBUG_H
#include "stdio.h"
#endif

using namespace CCD;

// initializing the data container
Regularity_Data::Regularity_Data(unsigned short window_number,
		unsigned short window_size)
	: Data_Container(), _window_number(window_number), _window_size(window_size)
{
	_stdev.clear();
	reset_current_window();
}

// add a feature value and add to the list of standard deviation when the window
// is full and reset the window.
void Regularity_Data::add_feature(double feature)
{
	_current_data++;
	rapid_stdev_calculation(feature);
    // check if the window is full
	if (_current_data >= _window_size)
	{
		add_stdev();
		reset_current_window();
	}	
}

// for the rapid standard deviation calculation we only need to maintain the sum
// and the sum of square (sum(x) and sum(x^2)
void Regularity_Data::rapid_stdev_calculation(double feature)
{
	_current_sum += feature;
	_current_sum_square += (feature * feature);
}

void Regularity_Data::add_stdev()
{
	// calculate the standard deviation
	// sqrt(N s2 - s1^2) / N
	// where s2 is the sum square and s1 is the sum
    double d = (_current_data * _current_sum_square) - 
		(_current_sum * _current_sum);
	double sigma = 0.0;
    if (d >= 0.0) sigma = sqrt(d) / _current_data;

	_stdev.push_back(sigma);
	// we only keep limited number of window
	if (_stdev.size() >= _window_number)
	{
		// remove the earlier data
		_stdev.erase(_stdev.begin(), 
			_stdev.begin() + (_stdev.size() - _window_number));
	}
}

// reset the window and the window
void Regularity_Data::reset_current_window()
{
	_current_data = 0;
	_current_sum_square = 0.0;
	_current_sum = 0.0;
}
