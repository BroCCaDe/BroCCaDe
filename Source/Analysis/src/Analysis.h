#ifndef AUX_PLUGIN_ANALYSIS_H
#define AUX_PLUGIN_ANALYSIS_H

#include "Data_Container.h"	// Data_Container
#include <memory>		// shared_ptr

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
