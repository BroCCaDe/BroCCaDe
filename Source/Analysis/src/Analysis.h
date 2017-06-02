#ifndef AUX_PLUGIN_ANALYSIS_H
#define AUX_PLUGIN_ANALYSIS_H

namespace CCD {

// the superclass for any feature analyzer
class FeatureAnalyzer
{
public:
	// add a feature and check whether the metric is calculated.
	// return true if metric is calculated, false otherwise.
	// the out parameter metric is -1 if metric is not calculated
	virtual bool add_feature(double iat, double *metric) = 0;
};

// simple feature analyzer which just take a feature and return it back
class NullAnalysis : public FeatureAnalyzer
{
public:
	virtual bool add_feature(double feature, double *metric) final
	{
		*metric = feature;
		return true;
	}
};

}

#endif
