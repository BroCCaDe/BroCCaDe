#include "Regularity_Data_Container.h"
#include <memory>
#include <math.h>

#ifdef DEBUG_H
#include "stdio.h"
#endif

using namespace CCD;


Regularity_Data::Regularity_Data(unsigned short window_number,
		unsigned short window_size)
	: Data_Container(), _window_number(window_number), _window_size(window_size)
{
	_stdev.clear();
	reset_current_window();
}

void Regularity_Data::add_feature(double feature)
{
//	printf("Regularity_Data::add_feature got here\n");
	_current_data++;
	rapid_stdev_calculation(feature);
	if (_current_data >= _window_size)
	{
		add_stdev();
		reset_current_window();
	}	
}

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
	double sigma = sqrt((_current_data * _current_sum_square) - 
		(_current_sum * _current_sum)) / _current_data;

	_stdev.push_back(sigma);
	// we only keep limited number of window
	if (_stdev.size() >= _window_number)
	{
		// remove the earlier data
		_stdev.erase(_stdev.begin(), 
			_stdev.begin() + (_stdev.size() - _window_number));
	}
}

void Regularity_Data::reset_current_window()
{
	_current_data = 0;
	_current_sum_square = 0.0;
	_current_sum = 0.0;
}
