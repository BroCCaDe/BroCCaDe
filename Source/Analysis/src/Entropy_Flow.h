#ifndef AUX_PLUGINS_ENTROPY_FLOW_H
#define AUX_PLUGINS_ENTROPY_FLOW_H

#include <vector>		// vector
#include "Histogram.h"		// Histogram
#include "Analysis.h"		// superclass

namespace CCD {

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
