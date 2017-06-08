#ifndef AUX_PLUGINS_ENTROPY_FLOW_H
#define AUX_PLUGINS_ENTROPY_FLOW_H

#include <vector>		// vector
#include "Histogram.h"
#include "Analysis.h"		// superclass
//#include "Bin_Strategy.h"	// Bin allocator

namespace CCD {

/*
// Flow analyzer which uses entropy analysis on a histogram of data
class Entropy : public FlowAnalyzer {
public:
	Entropy(unsigned int step_size, 
		std::shared_ptr<Bin_Strategy> binner);
	virtual ~Entropy() {};
protected:
	virtual double calculate_metric();
private:
};
*/

// Flow analyzer which uses entropy analysis on a histogram of data
class Entropy : public FeatureAnalyzer {
public:
	Entropy(std::shared_ptr<Histogram> data) : FeatureAnalyzer(data) {}
	virtual ~Entropy() {}

	virtual double calculate_metric();
private:
};

}

#endif
