#ifndef AUX_PLUGINS_FLOW_H
#define AUX_PLUGINS_FLOW_H

#include <vector>		// vector
#include <memory>		// unique_ptr
#include "Data_Container.h"	// Data_Container
#include "Analysis.h"		// superclass
#include "Bin_Strategy.h"	// bin allocation

namespace CCD {

class TempValue;

// global config for all flows. This should be set-up in the plugin before creating any Flow
class FlowConfig {
public:
	FlowConfig() {}
	unsigned int KS_window_size;			// window size for KS raw data
	unsigned int Regularity_window_number;		// number of window for regularity metric
	unsigned int Regularity_window_size;		// the size of each window for regularity metric
	unsigned int CCE_pattern_size;			// the length of pattern for CCE
	std::vector<unsigned int> step_sizes;		// step size for each calculation set
	unsigned int tag_count;				// maximum number of analysis tags (dynamic)
	unsigned int type_count;			// maximum number of data container type (static)
	unsigned int set_IDs;				// maximum known value of set_ID
	unsigned int number_of_analysis;		// maximum number of analysis (static)
	std::vector<Data_Container_Enum> 		// map from analysis ID to data container type
		map_analysis_data;			//   (static)

	// these ID should be treated as an enum, i.e. a static mapping from the analysis_ID enum in 
	// .bif file to each analysis.
	unsigned int KS_analysis;			// analysis ID for KS test
	unsigned int Entropy_analysis;			// analysis ID for Entropy
	unsigned int CCE_analysis;			// analysis ID for CCE
	unsigned int Regularity_analysis;		// analysis ID for Regularity
	unsigned int Autocorrelation_analysis;		// analysis ID for Autocorrelation
	unsigned int MultiModal_analysis;		// analysis ID for Multi-Modality
	unsigned int Null_analysis;			// analysis ID for null analysis

	// lags for autocorrelation calculation
	std::shared_ptr<std::vector<unsigned int> > Autocorrelation_lags;
	// normal data to test against. Used for KS test
	std::shared_ptr<std::vector<double> > KS_normal_data;
	// currently we only accommodate 1 binning strategy, used for Histogram and CCE pattern
	std::shared_ptr<Bin_Strategy> binner;
};

// the class that holds the data container and analysis per flow.
// Currently we store the analysis objects so that we don't have to create and destroy the analysis
// object for each analysis triggered.
// Each calculation set is clearly distinguished by the begin - end method call.
// The adding feature in between the begin - end pair is assumed to be in the context specified
// in the begin_adding_feature method. If the calculation is triggered, then the add_analysis
// method is also invoked.
class Flow {
public:
	Flow (std::shared_ptr<FlowConfig> config)
	{
		_data.clear();				// initialize the map to data containers
		_data.resize(config->set_IDs);
		_analysis.clear();			// initialize the map to the analysis
		_analysis.resize(config->set_IDs);
		_steps.resize(config->set_IDs, 0);	// initialize the steps for each calculation set
		_current_set_ID = -1;			
		_config = config;			// copy the reference to the config
	}

	void begin_adding_feature(unsigned int set_ID);	// begin
	// adding feature method identified by tag and list of analysis IDs.
	// The list of analysis will share the feature value, and the feature value will be added
	// to each analysis' data container (no duplicate)
	void add_feature(unsigned int tag, std::vector<unsigned int> aid, double feature);
	bool end_adding_feature();			// end

	std::vector<std::unique_ptr<TempValue> > get_result();
private:
	// trigger the analysis identified by the tag and analysis ID.
	// This should be called after we insert the data in add_feature
	void add_analysis(unsigned int tag, unsigned int aid);

	// map from (set_ID, tag, aid) to a data container
	std::vector< std::vector< std::vector <
		std::shared_ptr <Data_Container> > > > _data;
	// map from (set_ID, tag, aid) to an analysis object
	std::vector< std::vector< std::vector <
		std::shared_ptr <FeatureAnalyzer> > > > _analysis;
	std::shared_ptr<FlowConfig> _config;		// local reference to the global flow config
	std::vector<unsigned int> _steps;		// steps for each calculation set
	std::vector<std::unique_ptr<TempValue> > _result;
	int _current_set_ID;				// current calculation set ID
};

// used to store the value of the calculated metric with its accompanying analysis ID and input tag
class TempValue {
public:
	TempValue(double value, int aid, int tag) : _value(value), _aid(aid), _tag(tag) {}
	~TempValue() {}
	double _value;
	int _aid;
	int _tag;
};

}

#endif
