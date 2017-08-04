/*******************************************************************************\
* Copyright (c) 2017 by Hendra Gunadi (Hendra.Gunadi@murdoch.edu.au)            *
*                                                                               *
* Redistribution and use in source and binary forms, with or without            *
* modification, are permitted provided that the following conditions are met:   *
*                                                                               *
* (1) Redistributions of source code must retain the above copyright            *
*     notice, this list of conditions and the following disclaimer.             *
*                                                                               *
* (2) Redistributions in binary form must reproduce the above copyright         *
*     notice, this list of conditions and the following disclaimer in           *
*     the documentation and/or other materials provided with the                *
*     distribution.                                                             *
*                                                                               *
* (3) Neither the name of Hendra Gunadi and/or Murdoch University, nor          *
*     the names of contributors may be used to endorse or promote               *
*     products derived from this software without specific prior written        *
*     permission.                                                               *
*                                                                               *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"   *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE     *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE    *
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF          *
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS      *
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN       *
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)       *
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE    *
* POSSIBILITY OF SUCH DAMAGE.                                                   *
*                                                                               *
* Plugin.cc : Implements Plugin.h                                               *
\*******************************************************************************/

#include "Plugin.h"
#include "classifier_c45.h"
#include "Event.h"
#include "decisiontree.bif.h"

// Macro to check the boundary, i.e. the ID against the existing classifiers
#define CHECK_BOUND(ID) \
	if (_classifiers.find((unsigned int) ID) == _classifiers.end()) { \
		printf("Plugin::DecisionTree : Invalid ID %d (not found)\n", ID); \
		return -1; }

// Check whether a model specified by the ID has / has not been loaded
#define CHECK_MODEL(ID, op, fmt) \
	if (_classifiers[ID].get() op NULL) { \
		printf(fmt, ID); \
		return -1; }

// Send the actual event
#define SEND_EVENT(ID, conn_ID, c) \
	val_list* vl = new val_list; \
    vl->append(new Val(ID, TYPE_ENUM)); \
    RecordVal* conn_r = conn_ID->AsRecordVal(); \
    RecordVal* conn_ID_copy = new RecordVal(conn_id); \
	conn_ID_copy->Assign(0, new AddrVal(conn_r->Lookup(0)->AsAddr())); \
    PortVal* src_port = conn_r->Lookup(1)->AsPortVal(); \
	conn_ID_copy->Assign(1, new PortVal(src_port->Port(), src_port->PortType())); \
	conn_ID_copy->Assign(2, new AddrVal(conn_r->Lookup(2)->AsAddr())); \
    PortVal* dst_port = conn_r->Lookup(3)->AsPortVal(); \
	conn_ID_copy->Assign(3, new PortVal(dst_port->Port(), dst_port->PortType())); \
	vl->append(conn_ID_copy); \
	vl->append(new Val(c, TYPE_INT)); \
	mgr.QueueEvent(DecisionTree::class_result_event, vl);

// Sanity check for length that has to be equal
#define LENGTH_CHECK(length1, length2, err) \
	if (length1 != length2) { \
		printf(err); \
		return -1; }


namespace plugin { namespace Analysis_DecisionTree { Plugin plugin; } }

using namespace plugin::Analysis_DecisionTree;

plugin::Configuration Plugin::Configure()
	{
	plugin::Configuration config;
	config.name = "Analysis::DecisionTree";
	config.description = "<Insert description>";
	config.version.major = 0;
	config.version.minor = 1;

    _classifiers.clear();

	return config;
	}

int plugin::Analysis_DecisionTree::Plugin::LoadModel(Val* ID_val, StringVal* model_name)
{
	int ID = ID_val->AsEnum();
	CHECK_MODEL(ID, !=, "Plugin::DecisionTree : Model (%d) has been loaded\n");

	const char* model_file = (const char*) model_name->Bytes();
#ifdef DEBUG_H
	printf("model str : %s\n", model_file);
#endif
	
	// load the classifier and move the reference
	std::unique_ptr<c45_classifier> temp_classifier(new c45_classifier(model_file));
	_classifiers[ID].swap(temp_classifier);

    return 1;
}

int plugin::Analysis_DecisionTree::Plugin::Classify(Val* ID_val, Val* conn_ID, 
    Val* features, bool send_event)
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

    int c = _classifiers[ID]->c45_classify(features_double);

    //TODO: at the moment it is a fixed class (CC == 0), have to make sure that 0 is CC

    if (send_event)
    {
	    SEND_EVENT(ID, conn_ID, c);
    }
    
    return c;
}

int plugin::Analysis_DecisionTree::Plugin::Classify_with_strings(Val* ID_val, 
    Val* conn_ID, Val* features, Val* features_str, bool send_event)
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
			return -1;
		}
	}

    int c = _classifiers[ID]->c45_classify(features_double);

    if (send_event) 
    {
        SEND_EVENT(ID, conn_ID, c);
    }

    return c;
}

int plugin::Analysis_DecisionTree::Plugin::Classify_record(Val* ID_val, Val* conn_ID, 
    Val* features, bool send_event)
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

    int c = _classifiers[ID]->c45_classify(features_double);

    if (send_event)
    {
	    SEND_EVENT(ID, conn_ID, _classifiers[ID]->c45_classify(features_double));
    }

    return c;
}
