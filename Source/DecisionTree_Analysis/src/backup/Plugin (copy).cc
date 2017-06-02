#include "Plugin.h"
#include "diffuse_classifier_c45.h"
#include "classifier_c45.h"
#include "Event.h"
#include "decisiontree.bif.h"

namespace plugin { namespace Analysis_DecisionTree { Plugin plugin; } }

using namespace plugin::Analysis_DecisionTree;

enum classifier_tokens {
        TOK_MODEL_FILE = TOK_FEATURE_MOD_START,
};

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
	_cdata.resize(n);
	_model_loaded.resize(n, false);
	_init = true;
}

void plugin::Analysis_DecisionTree::Plugin::LoadModel(unsigned int ID, StringVal* model_name)
{
	if (ID > _count) {
		printf("Plugin::DecisionTree : Invalid ID %d (larger than %d)\n", ID, _count); 
		return;
	}
	if (_model_loaded[ID])
	{
		printf("Plugin::DecisionTree : Model (%d) has been loaded\n", ID);
		return;
	}

	const char* model_file = (const char*) model_name->Bytes();
	printf("model str : %s\n", model_file);

	_classifier[ID] = new c45_classifier(model_file);

	//register_classifier_module(&classifier_module_list, c45_module());

	/* open classifier model and initialise data structs */

	//classifier = find_classifier_module(classifier_module_list, algorithm);
	cnf_len = classifier->get_cnf_size();
	_cdata[ID].conf = malloc(cnf_len);

	classifier->parse_opts(TOK_OPTS_INIT, NULL, (struct di_oid *)_cdata[ID].conf);
	classifier->parse_opts(TOK_MODEL_FILE, model_file, (struct di_oid *)_cdata[ID].conf);
	model_len = classifier->load_model((struct di_oid *)_cdata[ID].conf, &model, &feature_str,
			&class_str);

	_cdata[ID].conf = realloc(_cdata[ID].conf, cnf_len + model_len); 
	memcpy(&((char *)_cdata[ID].conf)[cnf_len], model, model_len);

	_model_loaded[ID] = true;
}

void plugin::Analysis_DecisionTree::Plugin::Classify(unsigned int ID, Val* features)
{
	if (ID > _count) {
		printf("Plugin::DecisionTree : Invalid ID %d (larger than %d)\n", ID, _count); 
		return;
	}
	if (!_model_loaded[ID])
	{
		printf("Plugin::DecisionTree : Model (%d) has not been loaded\n", ID);
		return;
	}

	unsigned int i;
	vector<Val*> features_vec = *(features->AsVector());
	unsigned int fcnt = features_vec.size();
	double features_double[fcnt];

	for (i = 0; i < fcnt; i++)
		features_double[i] = features_vec[i]->AsDouble();

	int pred_class = c45_classify(&(_cdata[ID]), features_double, fcnt);

	val_list* vl = new val_list;
	vl->append(new Val(pred_class, TYPE_INT));
	mgr.QueueEvent(DecisionTree::class_result_event, vl);
}
