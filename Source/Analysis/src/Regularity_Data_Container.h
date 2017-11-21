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
* Regularity_Data_Container.h : data container to calculate regularity metric.  *
*       It is based on the rapid calculation formula                            *
*       sqrt(N * sum(feature^2) + sum(feature)^2) / N where N is the number of  *
*       data points. It calculate the standard deviation within each window.    *
\*******************************************************************************/

#ifndef AUX_PLUGINS_REGULARITY_DATA_CONTAINER_H
#define AUX_PLUGINS_REGULARITY_DATA_CONTAINER_H

#include <vector>                       // vector
#include "Data_Container.h"             // abstract Data_Container

namespace CCD {

class Regularity_Data : public Data_Container
{
public:
	Regularity_Data(unsigned short window_number, 
		unsigned short window_size);
	virtual ~Regularity_Data() {_stdev.clear();}

	virtual void add_feature(double feature);

	std::vector<double> get_stdev() {return _stdev;}
private:
	// store the sum and the sum square
	void rapid_stdev_calculation(double feature);
	// calculate standard deviation and add to _stdev
	void add_stdev();
	// reset _current_data, _current_sum, and _current_sum_square
	void reset_current_window();

	unsigned short _window_number;      // limit the number of standard deviation
	unsigned short _window_size;        // the window size
	std::vector<double> _stdev;         // moving window of standard deviations
	unsigned int _current_data;		
	double _current_sum;                // sum (feature)
	double _current_sum_square;         // sum (feature^2)
};

}

#endif
