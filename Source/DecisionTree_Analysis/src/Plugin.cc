#include "Plugin.h"
#include "classifier_c45.h"
#include "Event.h"
#include "decisiontree.bif.h"

// Macro to check the boundary, i.e. the ID against the existing classifiers
#define CHECK_BOUND(ID) \
	if ((unsigned int) ID > _classifiers.size()) { \
		printf("Plugin::DecisionTree : Invalid ID %d (larger than %lu)\n", ID, _classifiers.size()); \
		return; }

// Check whether a model specified by the ID has / has not been loaded
#define CHECK_MODEL(ID, op, fmt) \
	if (_classifiers[ID].get() op NULL) { \
		printf(fmt, ID); \
		return; }

// Send the actual event
#define SEND_EVENT(c) \
	val_list* vl = new val_list; \
	vl->append(new Val(c, TYPE_INT)); \
	mgr.QueueEvent(DecisionTree::class_result_event, vl);

// Sanity check for length that has to be equal
#define LENGTH_CHECK(length1, length2, err) \
	if (length1 != length2) { \
		printf(err); \
		return; }


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
	_classifiers.clear();
	_classifiers.resize(n);
}

void plugin::Analysis_DecisionTree::Plugin::LoadModel(Val* ID_val, StringVal* model_name)
{
	int ID = ID_val->AsEnum();
	CHECK_BOUND(ID);
	CHECK_MODEL(ID, !=, "Plugin::DecisionTree : Model (%d) has been loaded\n");

	const char* model_file = (const char*) model_name->Bytes();
#ifdef DEBUG_H
	printf("model str : %s\n", model_file);
#endif
	
	// load the classifier and move the reference
	std::unique_ptr<c45_classifier> temp_classifier(new c45_classifier(model_file));
	_classifiers[ID].swap(temp_classifier);
}

void plugin::Analysis_DecisionTree::Plugin::Classify(Val* ID_val, Val* features)
{
	int ID = ID_val->AsEnum();
	CHECK_BOUND(ID);
	CHECK_MODEL(ID, ==, "Plugin::DecisionTree : Model (%d) has not been loaded\n");

	vector<Val*> features_vec = *(features->AsVector());
	LENGTH_CHECK(features_vec.size(), _classifiers[ID]->get_feature_length(),
		"Plugin::DecisionTree : the length of the features are not the same\n"); 

	// construct the vector of double as the classifier input
	std::vector<double> features_double(_classifiers[ID]->get_feature_length());
	for (unsigned int i = 0; i < _classifiers[ID]->get_feature_length(); i++)
		features_double[i] = features_vec[i]->AsDouble();

	SEND_EVENT(_classifiers[ID]->c45_classify(features_double));
}

void plugin::Analysis_DecisionTree::Plugin::Classify_with_strings(Val* ID_val, Val* features, Val* features_str)
{
	int ID = ID_val->AsEnum();
	CHECK_BOUND(ID);
	CHECK_MODEL(ID, ==, "Plugin::DecisionTree : Model (%d) has not been loaded\n");

	vector<Val*> features_vec = *(features->AsVector());
	vector<Val*> features_str_vec = *(features_str->AsVector());
	LENGTH_CHECK(features_vec.size(), features_str_vec.size(), 
		"Plugin::DecisionTree : the length of the vectors are not the same\n");

	// construct the vector of double as the classifier input
	std::vector<double> features_double(_classifiers[ID]->get_feature_length());
	// naive searching, matching the required features against the feature strings
	for (unsigned int i = 0, j; i < _classifiers[ID]->get_feature_length(); i++)
	{
		std::string str = _classifiers[ID]->get_feature_str(i);
		for (j = 0; j < features_str_vec.size(); j++)
		{
			if (str.compare((const char*) features_str_vec[j]->AsStringVal()->Bytes()))
			{
				features_double[i] = features_vec[j]->AsDouble();
				break;
			}
		}
		if (j == features_str_vec.size()) // if the string is not found, then abort
		{
			printf("Plugin::DecisionTree : feature string is not found \"%s\"\n", str.c_str());
			return;
		}
	}

	SEND_EVENT(_classifiers[ID]->c45_classify(features_double));
}

void plugin::Analysis_DecisionTree::Plugin::Classify_record(Val* ID_val, Val* features)
{
	int ID = ID_val->AsEnum();
	CHECK_BOUND(ID);
	CHECK_MODEL(ID, ==, "Plugin::DecisionTree : Model (%d) has not been loaded\n");

	RecordVal* features_record = features->AsRecordVal();

	// construct the vector of double as the classifier input
	std::vector<double> features_double(_classifiers[ID]->get_feature_length());
	for (unsigned int i = 0; i < _classifiers[ID]->get_feature_length(); i++)
	{
		// if the string is not found in the record, then the program abort
		features_double[i] = features_record->Lookup(
			_classifiers[ID]->get_feature_str(i).c_str())->AsDouble();
	}

	SEND_EVENT(_classifiers[ID]->c45_classify(features_double));
}
