#ifndef AUX_PLUGINS_FLOW_H
#define AUX_PLUGINS_FLOW_H

#include <vector>		// vector
#include <memory>		// unique_ptr
#include "Data_Container.h"	// Data_Container
#include "Analysis.h"		// superclass
#include "Bin_Strategy.h"

namespace CCD {
/*
class FlowAnalyzer : public FeatureAnalyzer {
public:
	FlowAnalyzer(std::unique_ptr<Data_Container> data, unsigned int step_size);
	virtual ~FlowAnalyzer() {}
	
	// return true if metric is calculated, false otherwise
	// the out parameter metric is -1 if metric is not calculated
	virtual bool add_feature(double iat, double *metric) final;
	virtual void set_step_size(unsigned int step_size) {_step_size = step_size; _step = 0;}
protected:
	virtual double calculate_metric() = 0;	// child classes have to override this function
	std::unique_ptr<Data_Container> _data;	// polymorphic data container
	unsigned int _step;			// step and step size are used to check if the 
	unsigned int _step_size;		// metric calculation is triggered
};
*/

class TempValue;

class FlowConfig {
public:
	FlowConfig() : KS_window_size(0), Regularity_window_number(0), Regularity_window_size(0),
		CCE_pattern_size(0) {}
	unsigned int KS_window_size;
	unsigned int Regularity_window_number;
	unsigned int Regularity_window_size;
	unsigned int CCE_pattern_size;
	unsigned int tag_count;
	std::vector<unsigned int> step_sizes;
	unsigned int type_count;
	unsigned int set_IDs;
	unsigned int number_of_analysis;
	std::vector<Data_Container_Enum> map_analysis_data;
	unsigned int KS_analysis;
	unsigned int Entropy_analysis;
	unsigned int CCE_analysis;
	unsigned int Regularity_analysis;
	unsigned int Autocorrelation_analysis;
	unsigned int MultiModal_analysis;
	unsigned int Null_analysis;
	std::shared_ptr<std::vector<unsigned int> > Autocorrelation_lags;
	std::shared_ptr<std::vector<double> > KS_normal_data;
	// currently we only accommodate 1 bin strategy
	std::shared_ptr<Bin_Strategy> binner;
};

class Flow {
public:
	Flow (std::shared_ptr<FlowConfig> config)
	{
		_data.clear();
		_data.resize(config->set_IDs);
		_analysis.clear();
		_analysis.resize(config->set_IDs);
		_step = 0;
		_current_set_ID = -1;
		_config = config;
	}

	void begin_adding_feature(unsigned int set_ID);
	void add_feature(unsigned int tag, unsigned int aid, double feature);
	void add_analysis(unsigned int tag, unsigned int aid);
	bool end_adding_feature();

	std::vector<std::unique_ptr<TempValue> > get_result();
private:
	std::vector< std::vector< std::vector <
		std::shared_ptr <Data_Container> > > > _data;
	std::vector< std::vector< std::vector <
		std::shared_ptr <FeatureAnalyzer> > > > _analysis;
	std::shared_ptr<FlowConfig> _config;
	unsigned int _step;
	std::vector<std::unique_ptr<TempValue> > _result;
	int _current_set_ID;
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
