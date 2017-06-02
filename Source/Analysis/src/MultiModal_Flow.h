#ifndef AUX_PLUGINS_MULTIMODAL_FLOW_H
#define AUX_PLUGINS_MULTIMODAL_FLOW_H

#include <vector>		// vector
#include <memory>		// shared_ptr
#include "Bin_Strategy.h"	// abstract Bin allocator
#include "Flow.h"		// superclass

namespace CCD {

// Flow analyzer which uses a multimodal analysis on the possible symbols on
// the histogram
class MultiModal : public FlowAnalyzer {
public:
	MultiModal(unsigned int step_size, std::shared_ptr<Bin_Strategy> binner);
	virtual ~MultiModal() {};

protected:
	virtual double calculate_metric();
};

}

#endif
