#include <vector>
#include <memory>
#include "Flow.h"

//#define DEBUG_H

#ifdef DEBUG_H
#include <stdio.h>
#endif

using namespace CCD;

FlowAnalyzer::FlowAnalyzer(std::unique_ptr<Data_Container> data, unsigned int step_size)
{
	_data.swap(data);
	_step_size = step_size;
	_step = 0;
}

// Check if the metric calculation should be triggered after adding a feature value
bool FlowAnalyzer::add_feature(double feature, double* metric) 
{
	_data->add_feature(feature);

#ifdef DEBUG_H
	printf("Flow.cc: currently step %d\n", _step);
#endif
	
	// if the metric calculation is triggered, reset the step
	if (++_step >= _step_size)
	{
		*metric = calculate_metric();
		_step = 0;
		return true;
	}
	// the metric calculation is not yet triggered
	return false;
}
