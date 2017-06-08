#ifndef AUX_PLUGINS_REGULARITY_DATA_CONTAINER_H
#define AUX_PLUGINS_REGULARITY_DATA_CONTAINER_H

#include <vector>		// vector
#include "Data_Container.h"	// abstract Data_Container

namespace CCD {

class Regularity_Data : public Data_Container
{
public:
	Regularity_Data(unsigned short window_number, 
		unsigned short window_size);
	virtual ~Regularity_Data() {_stdev.clear();}

	// return a boolean indicating whether the calculation is triggered
	virtual void add_feature(double feature);

	std::vector<double> get_stdev() {return _stdev;}
private:
	// store the sum and the sum square
	void rapid_stdev_calculation(double feature);
	// calculate standard deviation and add to _stdev
	void add_stdev();
	// reset _current_data, _current_sum, and _current_sum_square
	void reset_current_window();

	unsigned short _window_number;		// limit the number of standard deviation
	unsigned short _window_size;		// the window size
	std::vector<double> _stdev;		// moving window of standard deviations
	unsigned int _current_data;		
	double _current_sum;			// sum (feature)
	double _current_sum_square;		// sum (feature^2)
};

}

#endif
