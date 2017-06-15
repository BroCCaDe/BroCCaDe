#ifndef AUX_PLUGINS_REGULARITY_FLOW_H
#define AUX_PLUGINS_REGULARITY_FLOW_H

#include <vector>			// vector
#include "Analysis.h"			// superclass
#include "Regularity_Data_Container.h"	// Regularity_Data

namespace CCD {

// Flow analyzer which uses entropy analysis on a histogram of data
class Regularity : public FeatureAnalyzer {
public:
	Regularity(std::shared_ptr<Regularity_Data> data) : FeatureAnalyzer(data) {}
	virtual ~Regularity() {};

	virtual double calculate_metric();
};

}

#endif
