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

	virtual void add_feature(double feature) {_data.push_back(feature);}

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
};

}

#endif
