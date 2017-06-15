
#ifndef BRO_PLUGIN_ANALYSIS_FEATUREANALYSIS_H
#define BRO_PLUGIN_ANALYSIS_FEATUREANALYSIS_H

#include <plugin/Plugin.h>
#include <unordered_map>		// unordered_map
#include <vector>			// vector
#include <memory>			// shared_ptr
#include "Flow.h"			// Flow
#include "Bin_Strategy.h"		// Bin allocation
#include "analysis.bif.h"

namespace plugin {
namespace Analysis_FeatureAnalysis {

using namespace BifEnum::FeatureAnalysis;

// NUMBER_OF_ANALYSIS must match the enum defined in analysis.bif
#define NUMBER_OF_ANALYSIS 7
// the following constants are arbitrary, as long as it is of reasonable integer value
#define DEFAULT_TAG_COUNT 3
#define DEFAULT_STEP_SIZE 10
#define DEFAULT_SET_IDS 3
#define DEFAULT_KS_WINDOW_SIZE 10
#define DEFAULT_REGULARITY_WINDOW_SIZE 10
#define DEFAULT_REGULARITY_WINDOW_NUMBER 1000
#define DEFAULT_CCE_PATTERN_SIZE 5


class Plugin : public ::plugin::Plugin
{
public:
	void RemoveConnection(StringVal* UID);

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
	// map from UID to Flow object
	std::unordered_map <std::string, std::shared_ptr<CCD::Flow> > _flow_dict;
	std::shared_ptr<CCD::FlowConfig> _flow_config;	// global config for all flows
	// temporary holder is useful so that we don't have to do the lookup multiple times for the same flow.
	std::shared_ptr<CCD::Flow> _current_flow; 	// temporary holder
	std::string* _current_UID;			// temporary holder's UID
	unsigned int _current_set_ID;			// temporary holder's set_ID
	Val* _current_conn_id;				// temporary holder's 4 network tuple
};

extern Plugin plugin;

}
}

#endif
