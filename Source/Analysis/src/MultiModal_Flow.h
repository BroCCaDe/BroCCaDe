#ifndef AUX_PLUGINS_MULTIMODAL_FLOW_H
#define AUX_PLUGINS_MULTIMODAL_FLOW_H

#include <vector>		// vector
#include <memory>		// shared_ptr
#include "Analysis.h"		// superclass
#include "Histogram.h"		// Histogram

namespace CCD {

// Flow analyzer which uses a multimodal analysis on the possible symbols on
// the histogram
class MultiModal : public FeatureAnalyzer {
public:
	MultiModal(std::shared_ptr<Histogram> data) : FeatureAnalyzer(data) {}
	virtual ~MultiModal() {};

	virtual double calculate_metric();
};

}

#endif
