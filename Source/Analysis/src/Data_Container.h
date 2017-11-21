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
* Data_Container.h : the superclass for the data container and raw data         *
*       container used for KS-test and autocorrelation.                         *
* Contents :                                                                    *
*   * class Data_Container : the superclass for data container. Mainly the child*
*           classes have to override the add_feature function.                  *
*   * class Raw_Data : a simple data container where the data is stored plainly *
*           as an array (vector) of double. Only keep a window of data.         *
\*******************************************************************************/

#ifndef AUX_PLUGINS_DATA_CONTAINER_H
#define AUX_PLUGINS_DATA_CONTAINER_H

#include <vector>	// vector
#include <stdio.h>	// printf

namespace CCD {

// Interface to the various data container
class Data_Container
{
public:
	virtual ~Data_Container() {};

	// return a boolean indicating whether the calculation is triggered
	virtual void add_feature(double feature) = 0;
};

// Simple data container which just stores the features in a vector.
// The moving window is parameterized by the size and the step-size.
// The step-size parameter indicates how many data points are flushed
// from the vector for each reset window, e.g. if we have a window size
// of 10 and step size 6, then calling the reset window will remove the
// data points from the vector until there are only 4 (10 - 6) data points left
class Raw_Data : public Data_Container
{
public: 
	// Construct the container by specifying the window size and the step size
	Raw_Data (unsigned int window_size, unsigned int step_size);
	virtual ~Raw_Data() {}

	virtual void add_feature(double feature) {
		_data.push_back(feature);
	}

	// reset the number of data points in the window down to (window_size - step_size)
	void reset_window();
	
	// Dynamically set the window_size, the window_size value must be larger than the step_size
	void set_window_size(unsigned int window_size) {
		if (window_size < _step_size) {
			printf("RawData: window size is smaller than step size\n"); 
			return;
		}
		_window_size = window_size;
	}
	unsigned int get_window_size() {return _window_size;}

	// Dynamically set the step_size, the window_size value must be larger than the step_size
	void set_step_size(unsigned int step_size) {
		if (_window_size < step_size) {
			printf("RawData: step size is bigger than the window size\n"); 
			return;
		}
		_step_size = step_size;
	}
	unsigned int get_step_size() {return _step_size;}

	// Get the data points in the form of a vector	
	std::vector<double> get_data() {return _data;}

	unsigned int get_data_len() {return _data.size();}
private:
	std::vector<double> _data;	// the vector containing the data series
	unsigned int _window_size; 	// the window size of the number of data
	unsigned int _step_size; 	// indicate the interval of window size, i.e. the
					            // number of data to empty each calculation triggered
//    unsigned int _plus_removed; 
};

// Simple data container which just store one value
class Null_Data : public Data_Container
{
public:
	Null_Data() : _value(0) {}
	virtual ~Null_Data() {}
	
	virtual void add_feature(double feature) {
		_value = feature;
	}
	double get_data() {return _value;}
private:
	double _value;
};

}

#endif
