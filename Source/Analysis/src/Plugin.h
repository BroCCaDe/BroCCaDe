
#ifndef BRO_PLUGIN_ANALYSIS_FEATUREANALYSIS_H
#define BRO_PLUGIN_ANALYSIS_FEATUREANALYSIS_H

#include <plugin/Plugin.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Flow.h"
#include "Bin_Strategy.h"
#include "analysis.bif.h"

namespace plugin {
namespace Analysis_FeatureAnalysis {

using namespace BifEnum::FeatureAnalysis;

// NUMBER_OF_ANALYSIS must match the enum defined in analysis.bif
#define NUMBER_OF_ANALYSIS 7
/* the following constants are arbitrary, as long as it is of reasonable integer value */
#define DEFAULT_TAG_COUNT 3
#define DEFAULT_STEP_SIZE 10
#define DEFAULT_SET_IDS 3
#define DEBUG

//class RequestValue;

class Plugin : public ::plugin::Plugin
{
public:
	void RemoveConnection(StringVal* UID);
/*
	void RegisterAnalysis(StringVal* UID, double feature, Val* aid, Val* tag);
	void CalculateMetric(Val* set_ID);
*/
	void RegisterAnalysis(StringVal* UID, Val* Set_ID, Val* conn_ID);
	void AddFeature(StringVal* UID, double feature, Val* aid_val, Val* tag_val);
	void CalculateMetric();

	void SetStepSize(Val* Set_ID, unsigned int step_size);
	Val* Extract(Val* v, Val* aid, Val* tag);
	Val* ExtractVector(Val* v);
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
private:
	/*
	// the map also has to take into account the analysis ID and tag (enums)
	std::unordered_map <std::string, std::shared_ptr<std::vector <
		std::vector < std::vector <
		std::shared_ptr<CCD::FeatureAnalyzer> > > > > > _flow_dict;*/
	std::unordered_map <std::string, std::shared_ptr<CCD::Flow> > _flow_dict;
	//std::vector<std::unique_ptr<RequestValue>> _request_list;
	std::shared_ptr<CCD::FlowConfig> _flow_config;
	std::shared_ptr<CCD::Flow> _current_flow; 	// temporary holder
	std::string* _current_UID;		// temporary holder's UID
	unsigned int _current_set_ID;		// temporary holder's set_ID
	Val* _current_conn_id;			// temporary holder's 4 network tuple
/*
	std::shared_ptr<std::vector<double>> _normal_data;
	// currently we only accommodate 1 bin strategy
	std::shared_ptr<CCD::Bin_Strategy> _binner;
	unsigned int _tag_count;	
	unsigned int _set_IDs;	
	// static NullAnalysis : no need for per flow object
	std::shared_ptr<CCD::NullAnalysis> _null_analysis;
	unsigned int _step_size;
*/
};

/*
// used to store the value of the calculated metric with its accompanying analysis ID and input tag
class TempValue {
public:
	TempValue(double value, Analysis_ID aid, int tag) : _value(value), _aid(aid), _tag(tag) {}
	~TempValue() {}
	double _value;
	Analysis_ID _aid;
	int _tag;
};

// used to store the analysis request
class RequestValue : public TempValue {
public:
	RequestValue(std::string *UID, double value, Analysis_ID aid, int tag)
		: TempValue(value, aid, tag), _UID(UID) {}
	~RequestValue() {delete _UID;}
	std::string *_UID;
};
*/
extern Plugin plugin;

}
}

#endif
