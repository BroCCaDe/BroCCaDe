#include <vector>		// vector
#include "Data_Container.h"

using namespace CCD;

Raw_Data::Raw_Data(unsigned int window_size, unsigned int step_size)
{
	_data.clear();
	_step_size = step_size;
	_window_size = window_size;
}

void Raw_Data::reset_window()
{
	unsigned int _data_len = _data.size();
	// check if the data needs to be flushed
	if (_data_len >= _window_size)
	{
		// if yes, then flush down to (window_size - step_size)
		unsigned int removed_data = _data_len - (_window_size - _step_size);
		_data.erase (_data.begin(), _data.begin() + removed_data);
	}
}
