#ifndef AUX_PLUGINS_CCE_FLOW_H
#define AUX_PLUGINS_CCE_FLOW_H

#include <vector>		// vector
#include <memory>		// shared_ptr
#include "Flow.h"		// superclass (flow analyzer)
#include "Bin_Strategy.h"	// bin allocator

namespace CCD {

// CCE flow analyzer, based from the description in 
// "An Entropy-Based Approach to Detecting Covert Timing Channels
// by Steven Gianvecchio and Haining Wang
class CCE : public FlowAnalyzer {
public:
	// Constructor: we need to specify the pattern size and the bin allocation strategy
	// The internal of the data container is represented as a tree of depth equal to the
	// pattern length. On each node, possible children are possible bin values.
	CCE (unsigned short step_size, unsigned short pattern_size, 
		std::shared_ptr<Bin_Strategy> binner);
	virtual ~CCE () {};
protected:
	// We calculate the entropy value by calculating conditional entropy on each level of
	// the tree. In particular, we calculate the minimum of 
	//	CCE (Xm|X1,...,Xm-1) = CE(Xm|X1,...,Xm-1) + perc(Xm) * EN(X1)
	// among patterns of length 1, ..., m
	// where EN(X1) is the entropy for the first level of the tree,
	// perc(Xm) is the percentage of unique patterns of length m,
	// and CE(Xm|X1,...,Xm-1) is the conditional entropy 
	virtual double calculate_metric();
};

}

#endif
