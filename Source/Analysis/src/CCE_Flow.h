#ifndef AUX_PLUGINS_CCE_FLOW_H
#define AUX_PLUGINS_CCE_FLOW_H

#include <vector>			// vector
#include <memory>			// shared_ptr
#include "Analysis.h"			// superclass (feature analyzer)
#include "Pattern_Data_Container.h"	// Pattern_Data

namespace CCD {

// CCE flow analyzer, based from the description in 
// "An Entropy-Based Approach to Detecting Covert Timing Channels
// by Steven Gianvecchio and Haining Wang
class CCE : public FeatureAnalyzer {
public:
	// Constructor: we need to specify the pattern size and the bin allocation strategy

	CCE (std::shared_ptr<Pattern_Data> data) : FeatureAnalyzer(data) {}
	virtual ~CCE () {};

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
