
#ifndef BRO_PLUGIN_TRAINING_BIN
#define BRO_PLUGIN_TRAINING_BIN

#include <plugin/Plugin.h>
#include "Bin_Training.h"
#include <vector>
#include <unordered_map>

#define DEFAULT_TAG_COUNT 3
#define TRAINING_DATA_NUMBER 100
#define DEFAULT_BIN_COUNT 5

namespace plugin {
namespace Training_Bin {

class Plugin : public ::plugin::Plugin
{
public:
	void RemoveConnection(StringVal* UID);
	void add_feature(StringVal* UID, double feature, Val* tag);
protected:
	// Overridden from plugin::Plugin.
private:
	// the map also has to take into account the analysis ID and tag (enums)
	std::unordered_map <std::string, std::vector <IntervalTraining*>* >* _flow_dict;
	virtual plugin::Configuration Configure();
	unsigned int _tag_count;
	unsigned int _bin_count;
};

extern Plugin plugin;

}
}

#endif
