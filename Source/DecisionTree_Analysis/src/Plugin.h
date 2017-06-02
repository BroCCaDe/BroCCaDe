
#ifndef BRO_PLUGIN_ANALYSIS_DECISIONTREE
#define BRO_PLUGIN_ANALYSIS_DECISIONTREE

#include <plugin/Plugin.h>
#include <vector>
#include <memory>
#include "classifier_c45.h"

namespace plugin {
namespace Analysis_DecisionTree {

class Plugin : public ::plugin::Plugin
{
public:
	void Init(unsigned int n);
	void LoadModel(Val* ID_val, StringVal* model_name);
	void Classify(Val* ID_val, Val* features);
	void Classify_with_strings(Val* ID_val, Val* features, Val* features_str);
	void Classify_record(Val* ID_val, Val* features);
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
private:
	std::vector<std::unique_ptr<c45_classifier>> _classifiers;
	std::vector<bool> _model_loaded;
	unsigned int _count;
	bool _init;
};

extern Plugin plugin;

}
}

#endif
