#ifndef AUX_PLUGINS_FLOW_H
#define AUX_PLUGINS_FLOW_H

#include <vector>		// vector
#include <memory>		// unique_ptr
#include "Data_Container.h"	// Data_Container
#include "Analysis.h"		// superclass

namespace CCD {

class FlowAnalyzer : public FeatureAnalyzer {
public:
	FlowAnalyzer(std::unique_ptr<Data_Container> data, unsigned int step_size);
	virtual ~FlowAnalyzer() {}
	
	// return true if metric is calculated, false otherwise
	// the out parameter metric is -1 if metric is not calculated
	virtual bool add_feature(double iat, double *metric) final;
	virtual void set_step_size(unsigned int step_size) {_step_size = step_size; _step = 0;}
protected:
	virtual double calculate_metric() = 0;	// child classes have to override this function
	std::unique_ptr<Data_Container> _data;	// polymorphic data container
	unsigned int _step;			// step and step size are used to check if the 
	unsigned int _step_size;		// metric calculation is triggered
};

}

#endif
