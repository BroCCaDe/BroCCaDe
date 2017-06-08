
#ifndef BRO_PLUGIN_FEATUREEXTRACTION_IAT
#define BRO_PLUGIN_FEATUREEXTRACTION_IAT

#include <plugin/Plugin.h>
#include <unordered_map>

namespace plugin {
namespace FeatureExtraction_IAT {

class Plugin : public ::plugin::Plugin
{
public:
	// calculate the inter arrival time. This inherently implies
	// that the first packet will not generate an event
	void ExtractFeature(StringVal* UID, Val* id, double duration);
	// whenever Bro removes a flow, we also remove the floe
	void RemoveConnection(StringVal* UID);
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
private:
	// maintain the last duration of each flow
	std::unordered_map<std::string, double>* _flow_dict;	
};

extern Plugin plugin;

}
}

#endif
