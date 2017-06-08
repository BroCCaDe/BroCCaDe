#ifndef AUX_PLUGINS_REGULARITY_FLOW_H
#define AUX_PLUGINS_REGULARITY_FLOW_H

#include <vector>		// vector
#include "Analysis.h"		// superclass
//#include "Bin_Strategy.h"	// Bin allocator
#include "Regularity_Data_Container.h"

namespace CCD {

/*
// Flow analyzer which uses entropy analysis on a histogram of data
class Regularity : public FlowAnalyzer {
public:
	Regularity(unsigned int step_size, unsigned short window_number, 
		unsigned short window_size);
	virtual ~Regularity() {};
protected:
	virtual double calculate_metric();
};
*/

// Flow analyzer which uses entropy analysis on a histogram of data
class Regularity : public FeatureAnalyzer {
public:
	Regularity(std::shared_ptr<Regularity_Data> data) : FeatureAnalyzer(data) {}
	virtual ~Regularity() {};

	virtual double calculate_metric();
};

}

#endif
