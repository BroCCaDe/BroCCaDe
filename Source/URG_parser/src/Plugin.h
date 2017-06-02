
#ifndef BRO_PLUGIN_FEATUREEXTRACTION_URGENTPOINTER
#define BRO_PLUGIN_FEATUREEXTRACTION_URGENTPOINTER

#include <plugin/Plugin.h>

namespace plugin {
namespace FeatureExtraction_UrgentPointer {

class Plugin : public ::plugin::Plugin
{
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
	virtual void HookSetupAnalyzerTree(Connection *conn);
};

extern Plugin plugin;

}
}

#endif
