#ifndef AUX_PLUGIN_ANALYSIS_H
#define AUX_PLUGIN_ANALYSIS_H

#include "Data_Container.h"
#include <memory>

namespace CCD {

// this has to match how many data countainer types there are
const int DATA_CONTAINER_TYPE_COUNT = 5;

enum Data_Container_Enum
{
	RAW_DATA = 0,
	HISTOGRAM_DATA = 1,
	PATTERN_DATA = 2,
	REGULARITY_DATA = 3,
	NULL_DATA = 4
};

/*
// map the analysis ID to Data_Container_Enum
const Data_Container_Enum _map_analysis_data[] = {RAW_DATA, HISTOGRAM_DATA, PATTERN_DATA, 
	NULL_DATA, HISTOGRAM_DATA, RAW_DATA, REGULARITY_DATA};
*/

// the superclass for any feature analyzer
/*
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
*/

class FeatureAnalyzer
{
public:
	FeatureAnalyzer(std::shared_ptr<Data_Container> data) : _data(data) {}
	virtual ~FeatureAnalyzer() {}

	virtual double calculate_metric() = 0;
protected:
	std::shared_ptr<Data_Container> _data;
};

class NullAnalysis : public FeatureAnalyzer
{
public:
	NullAnalysis(std::shared_ptr<Null_Data> data) : FeatureAnalyzer(data) {}
	virtual ~NullAnalysis() {}

	virtual double calculate_metric()
	{
		return (static_cast<Null_Data*>(_data.get()))->get_data();
	}
};


}

#endif
