#include "Plugin.h"
#include "classifier_c45.h"
#include "Event.h"
#include "decisiontree.bif.h"

#define CHECK_BOUND(ID) \
	if ((unsigned int) ID > _count) { \
		printf("Plugin::DecisionTree : Invalid ID %d (larger than %d)\n", ID, _count); \
		return; \
	}

namespace plugin { namespace Analysis_DecisionTree { Plugin plugin; } }

using namespace plugin::Analysis_DecisionTree;

plugin::Configuration Plugin::Configure()
	{
	plugin::Configuration config;
	config.name = "Analysis::DecisionTree";
	config.description = "<Insert description>";
	config.version.major = 0;
	config.version.minor = 1;

	return config;
	}

void plugin::Analysis_DecisionTree::Plugin::Init(unsigned int n)
{
	if (_init) {printf("Plugin::DecisionTree : already initialized\n"); return;}

	_count = n;
	_classifiers.resize(n);
	_model_loaded.resize(n, false);
	_init = true;
}

void plugin::Analysis_DecisionTree::Plugin::LoadModel(Val* ID_val, StringVal* model_name)
{
	int ID = ID_val->AsEnum();
	CHECK_BOUND(ID);

	if (_model_loaded[ID])
	{
		printf("Plugin::DecisionTree : Model (%d) has been loaded\n", ID);
		return;
	}

	const char* model_file = (const char*) model_name->Bytes();
	printf("model str : %s\n", model_file);

	std::unique_ptr<c45_classifier> temp_classifier(new c45_classifier(model_file));
	_classifiers[ID].swap(temp_classifier);

	_model_loaded[ID] = true;
}

void plugin::Analysis_DecisionTree::Plugin::Classify(Val* ID_val, Val* features)
{
	int ID = ID_val->AsEnum();
	CHECK_BOUND(ID);

	if (!_model_loaded[ID])
	{
		printf("Plugin::DecisionTree : Model (%d) has not been loaded\n", ID);
		return;
	}

	unsigned int i;
	vector<Val*> features_vec = *(features->AsVector());
	unsigned int fcnt = features_vec.size();
	std::vector<double> features_double(fcnt);

	for (i = 0; i < fcnt; i++)
		features_double[i] = features_vec[i]->AsDouble();

	int pred_class = _classifiers[ID]->c45_classify(features_double);

	val_list* vl = new val_list;
	vl->append(new Val(pred_class, TYPE_INT));
	mgr.QueueEvent(DecisionTree::class_result_event, vl);
}

void plugin::Analysis_DecisionTree::Plugin::Classify_with_strings(Val* ID_val, Val* features, Val* features_str)
{
	int ID = ID_val->AsEnum();
	CHECK_BOUND(ID);

	if (!_model_loaded[ID])
	{
		printf("Plugin::DecisionTree : Model (%d) has not been loaded\n", ID);
		return;
	}

	unsigned int i, j;
	unsigned int fcnt = _classifiers[ID]->get_feature_length();
	vector<Val*> features_vec = *(features->AsVector());
	unsigned int features_len = features_vec.size();
	vector<Val*> features_str_vec = *(features_str->AsVector());
	unsigned int features_str_len = features_str_vec.size();
	// sanity check
	if (features_len != features_str_len)
	{
		printf("Plugin::DecisionTree : the length of the vectors are not the same\n");
		return;
	}

	std::vector<double> features_double(fcnt);
	// naive searching, matching the feature strings
	for (i = 0; i < fcnt; i++)
	{
		std::string str = _classifiers[ID]->get_feature_str(i);
		for (j = 0; j < features_str_len; j++)
		{
			if (str.compare((const char*) features_str_vec[j]->AsStringVal()->Bytes()))
			{
				features_double[i] = features_vec[j]->AsDouble();
				break;
			}
		}
		if (j == features_str_len) // if the string is not found
		{
			printf("Plugin::DecisionTree : feature string is not found \"%s\"\n", str.c_str());
			return;
		}
	}

	int pred_class = _classifiers[ID]->c45_classify(features_double);

	val_list* vl = new val_list;
	vl->append(new Val(pred_class, TYPE_INT));
	mgr.QueueEvent(DecisionTree::class_result_event, vl);
}

void plugin::Analysis_DecisionTree::Plugin::Classify_record(Val* ID_val, Val* features)
{
	int ID = ID_val->AsEnum();
	CHECK_BOUND(ID);

	if (!_model_loaded[ID])
	{
		printf("Plugin::DecisionTree : Model (%d) has not been loaded\n", ID);
		return;
	}

	unsigned int i;
	unsigned int fcnt = _classifiers[ID]->get_feature_length();
	RecordVal* features_record = features->AsRecordVal();
	std::vector<double> features_double(fcnt);
	for (i = 0; i < fcnt; i++)
	{
		// if the string is not found in the record, then the program abort
		features_double[i] = features_record->Lookup(
			_classifiers[ID]->get_feature_str(i).c_str())->AsDouble();
	}

	int pred_class = _classifiers[ID]->c45_classify(features_double);

	val_list* vl = new val_list;
	vl->append(new Val(pred_class, TYPE_INT));
	mgr.QueueEvent(DecisionTree::class_result_event, vl);
}
