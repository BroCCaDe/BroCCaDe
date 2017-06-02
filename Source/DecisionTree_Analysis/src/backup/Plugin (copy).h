
#ifndef BRO_PLUGIN_ANALYSIS_DECISIONTREE
#define BRO_PLUGIN_ANALYSIS_DECISIONTREE

#include <plugin/Plugin.h>
#include <vector>
#include "diffuse.h"

namespace plugin {
namespace Analysis_DecisionTree {

class Plugin : public ::plugin::Plugin
{
public:
	void Init(unsigned int n);
	void LoadModel(unsigned int ID, StringVal* model_name);
	void Classify(unsigned int ID, Val* feature);
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
private:
	std::vector<struct cdata> _cdata;
	std::vector<bool> _model_loaded;
	unsigned int _count;
	bool _init;
};

extern Plugin plugin;

}
}

#endif
