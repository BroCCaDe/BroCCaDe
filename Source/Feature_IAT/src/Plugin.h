
#ifndef BRO_PLUGIN_FEATUREEXTRACTION_IAT
#define BRO_PLUGIN_FEATUREEXTRACTION_IAT

#include <plugin/Plugin.h>
#include <unordered_map>

namespace plugin {
namespace FeatureExtraction_IAT {

class Plugin : public ::plugin::Plugin
{
public:
	void ExtractFeature(StringVal* UID, double duration);
	void RemoveConnection(StringVal* UID);
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
private:
	std::unordered_map<std::string, double>* _flow_dict;
};

extern Plugin plugin;

}
}

#endif
