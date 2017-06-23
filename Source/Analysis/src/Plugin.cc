/*******************************************************************************\
* Copyright (c) 2017 by Hendra Gunadi (Hendra.Gunadi@murodch.edu.au)            *
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
\*******************************************************************************/

#include "Plugin.h"
#include "analysis.bif.h"
#include "Flow.h"			// Flow
#include "Bin_Strategy_Interval.h"	// abstract Bin allocator
#include <vector>			// vector
#include <unordered_map>		// unordered_map
#include <iostream>			
#include <cfloat>			// DBL_MAX
#include <fstream>			// ifstream
#include <memory>			// shared_ptr and unique_ptr

//#define DEBUG_H

#ifdef DEBUG_H
#include <stdio.h>
#endif

// this is mainly used to access the dictionary. During the development, there 
// are changes to design decision so it's easier to just change here as opposed to
// all over the code
#define ACCESS_1(arr, idx_1) (*arr)[idx_1]
#define ACCESS_2(arr, idx_1, idx_2) (*arr)[idx_1][idx_2] //  (*((*arr)[idx_1]))[idx_2]
#define ACCESS_3(arr, idx_1, idx_2, idx_3) (*arr)[idx_1][idx_2][idx_3] // (*((*((*arr)[idx_1]))[idx_2]))[idx_3]

namespace plugin { namespace Analysis_FeatureAnalysis { Plugin plugin; } }

using namespace plugin::Analysis_FeatureAnalysis;

using namespace BifEnum::FeatureAnalysis;

using namespace CCD;


plugin::Configuration Plugin::Configure()
	{
	plugin::Configuration config;
	config.name = "Analysis::FeatureAnalysis";
	config.description = "<Insert description>";
	config.version.major = 0;
	config.version.minor = 1;

	// initialize the config data for the flow analysis
	_flow_config = std::shared_ptr<FlowConfig> (new FlowConfig());
	_flow_config->binner = std::shared_ptr<Bin_Strategy_Interval> 
		(new Bin_Strategy_Interval());
	_flow_config->KS_normal_data = std::shared_ptr<std::vector<double> > 
		(new std::vector<double>());
	_flow_config->Autocorrelation_lags = std::shared_ptr<
		std::vector<unsigned int> > (new std::vector<unsigned int>());

	_flow_config->KS_window_size = DEFAULT_KS_WINDOW_SIZE;		
	_flow_config->Regularity_window_number = DEFAULT_REGULARITY_WINDOW_NUMBER;  
	_flow_config->Regularity_window_size = DEFAULT_REGULARITY_WINDOW_SIZE;	
	_flow_config->CCE_pattern_size = DEFAULT_CCE_PATTERN_SIZE;
	_flow_config->tag_count = DEFAULT_TAG_COUNT;
	_flow_config->step_sizes.resize(DEFAULT_SET_IDS);
	// hard coded for now: step size for URG analysis is for every packet
	_flow_config->step_sizes[0] = 1; 		
	// hard coded for now
	_flow_config->step_sizes[1] = DEFAULT_STEP_SIZE;
	// hard coded for now: step size for PTunnel analysis is for every ICMP payload
	_flow_config->step_sizes[2] = 1; 		

	_flow_config->set_IDs = DEFAULT_SET_IDS;
	_flow_config->type_count = DATA_CONTAINER_TYPE_COUNT;
	_flow_config->number_of_analysis = NUMBER_OF_ANALYSIS;

	_flow_config->KS_analysis = KS_ANALYSIS;
	_flow_config->Entropy_analysis = ENTROPY_ANALYSIS;
	_flow_config->CCE_analysis = CCE_ANALYSIS;
	_flow_config->Autocorrelation_analysis = AUTOCORRELATION_ANALYSIS;
	_flow_config->MultiModal_analysis = MULTIMODAL_ANALYSIS;
	_flow_config->Regularity_analysis = REGULARITY_ANALYSIS;
	_flow_config->Null_analysis = NULL_ANALYSIS;

	_flow_config->map_analysis_data.resize(NUMBER_OF_ANALYSIS);
	_flow_config->map_analysis_data[KS_ANALYSIS] = RAW_DATA;
	_flow_config->map_analysis_data[ENTROPY_ANALYSIS] = HISTOGRAM_DATA;
	_flow_config->map_analysis_data[CCE_ANALYSIS] = PATTERN_DATA;
	_flow_config->map_analysis_data[NULL_ANALYSIS] = NULL_DATA;
	_flow_config->map_analysis_data[MULTIMODAL_ANALYSIS] = HISTOGRAM_DATA;
	_flow_config->map_analysis_data[AUTOCORRELATION_ANALYSIS] = RAW_DATA;
	_flow_config->map_analysis_data[REGULARITY_ANALYSIS] = REGULARITY_DATA;
	
	// populate the bData_Container_Enum _map_analysis_data[]in intervals and the normal data
	Bin_Strategy_Interval* ptr = static_cast<Bin_Strategy_Interval*> (_flow_config->binner.get());

	std::string file_name_interval = "Interval_data_CoxW2n3uaB9oupwtGj_0";
	std::string file_name = "KS_data_C1oB2c1SH6sz8cuQdc_0";

	// reading intervals from the input file
	// the format of the file is
	// <number of intervals>
	// <lower bound> <upper bound>
	// ...
	ifstream f, f_int;	// input file stream
	std::string line;	// temporary string to read the file
	unsigned int N;		// number of intervals in the training data

  	f_int.open (file_name_interval);
	if (!f_int.is_open()) {cout << "(data) can't open file" << file_name_interval << "\n"; return config;}
	
	if ( getline (f_int, line) )
	{
		N = stoul(line);
		for (unsigned int i = 0; i < N; i++)
		{
			getline(f_int, line);
			try
			{
				double left; 
				double right; 
				if (sscanf(line.c_str(), "%lf %lf", &left, &right) < 2)
				{
					cout << "Plugin.cc: can't read 2 doubles from the line \"" << 
						line << "\"\n";
					continue;
				}
				ptr->add_interval(left, right);
			}
			catch (std::invalid_argument& e)
			{
				cout << e.what() << "\n";
			}
		}
	}
	f.close();

	// reading series of feature from the input file
	// the format of Data_Container_Enum _map_analysis_data[]the file is
	// <number of data points>
	// <interarrival time>
	// ...
	f.open (file_name);
	if (!f.is_open()) {cout << "(data) can't open file" << file_name << "\n"; return config;}
	
	if ( getline (f, line) )
	{
		N = stoul(line);
		for (unsigned int i = 0; i < N; i++)
		{
			getline(f, line);
			try
			{
				_flow_config->KS_normal_data->push_back(stod(line));
			}
			catch (std::invalid_argument& e)
			{
				cout << e.what() << "\n";
			}
		}
	}
	f.close();

	return config;
	}

// whenever Bro decided to clean a connection, we also flush the connection
// from the dictionary
void plugin::Analysis_FeatureAnalysis::Plugin::RemoveConnection(StringVal* UID)
{
	//BroString* newStr = new BroString(*(UID->AsString()));
	std::string UID_str((const char*) UID->Bytes());
	_flow_dict.erase(UID_str);
}

// set the step size for all future Flow
void plugin::Analysis_FeatureAnalysis::Plugin::SetStepSize(Val* Set_ID, unsigned int step_size)
{
	int set_ID = Set_ID->AsEnum();
	if ((unsigned int) set_ID >= _flow_config->set_IDs) 
	{
		_flow_config->set_IDs = (unsigned int) set_ID + 1;
		_flow_config->step_sizes.resize(_flow_config->set_IDs);
	}
	_flow_config->step_sizes[set_ID] = step_size;
};

// register the request for analysis which is distinguished by its analysis ID and tag
void plugin::Analysis_FeatureAnalysis::Plugin::RegisterAnalysis(StringVal* UID, Val* Set_ID, Val* conn_ID)
{
	std::string *UID_str = new std::string((const char*) UID->Bytes());	
	int set_ID = Set_ID->AsEnum();

	if ((unsigned int) set_ID >= _flow_config->set_IDs) 
	{
		_flow_config->set_IDs = (unsigned int) set_ID + 1;
		_flow_config->step_sizes.resize(_flow_config->set_IDs);
		_flow_config->step_sizes[set_ID] = DEFAULT_STEP_SIZE;
	}

	std::unordered_map <std::string, std::shared_ptr<Flow> >::iterator 
			got = _flow_dict.find(*UID_str);

	std::shared_ptr<Flow> flow;
	// check if we have the flow in the dictionary
	if (got == _flow_dict.end())
	{
#ifdef DEBUG_H
		printf("creating new flow ID: %s\n", UID_str->c_str());
#endif
		// if not then create a new entry
		flow = std::shared_ptr<Flow> (new Flow(_flow_config));
		pair<std::unordered_map<std::string, std::shared_ptr<Flow> >::iterator, bool>
			result = _flow_dict.emplace(*UID_str, flow);
		
		// and check that the entry is created successfully
		if (!result.second) 
			printf("error in creating new analysis per flow\n");
	}
	else
	{ 
		flow = got->second;
	}

	// remove last reference to the UID string and conn_ID
	delete(_current_UID);
	delete(_current_conn_id);

	// set the current temporary references
	_current_flow = flow;
	_current_UID = UID_str;
	_current_conn_id = conn_ID->Clone();
	_current_set_ID = set_ID;

	// indicate the flow begin
	flow->begin_adding_feature(set_ID);
}

// register the request for analysis which is distinguished by its analysis ID and tag
void plugin::Analysis_FeatureAnalysis::Plugin::AddFeature(StringVal* UID, double feature,
	Val* aid_val, Val* tag_val)
{
	std::string *UID_str = new std::string((const char*) UID->Bytes());	
	if (UID_str->compare(*_current_UID))
	{
		printf("error: current UID is not the same as the input UID\n");
		return;
	}
	delete(UID_str);	// we have no more use for the UID_str

	int tag = tag_val->AsEnum();

	// sanity check for tag
	if (tag < 0)
	{
		printf("error: invalid Tag %d\n", tag);
		return;
	}

	if ((unsigned int) tag >= _flow_config->tag_count) {_flow_config->tag_count = (unsigned int) tag + 1;}

	// get the analysis ID from the aid_val
	std::vector<Val*>* aid_vector_val = aid_val->AsVector();
	std::vector<unsigned int> aid_vector(aid_vector_val->size());
	for (unsigned int i = 0; i < aid_vector_val->size(); i++)
	{
		unsigned int aid = (*aid_vector_val)[i]->AsEnum();
		// sanity check for aid
		if (aid < 0 || aid > _flow_config->number_of_analysis)
		{
			printf("error: invalid analysis ID %d\n", aid);
			return;
		}
		aid_vector[i] = aid;
	}

	// add the feature to the current flow
	_current_flow->add_feature(tag, aid_vector, feature);
}


// Act upon the analysis request
void plugin::Analysis_FeatureAnalysis::Plugin::CalculateMetric()
{
	unsigned int index;
	bool metric_calculated = _current_flow->end_adding_feature();

	// send the actual event if metric calculation is triggered
	if (metric_calculated) 
	{
		// get the list of metrics from the Flow object
		std::vector<std::unique_ptr<CCD::TempValue>> return_list = _current_flow->get_result();

		val_list* vl = new val_list;
		vl->append(new Val(_current_set_ID, TYPE_ENUM));
		vl->append(new VectorVal(internal_type("result_vector")->AsVectorType()));
		// add the metric result to the vector for event argument
		for (unsigned long i = 0; i < return_list.size(); i++)
		{
			RecordVal* r = new RecordVal(internal_type("analysis_result")->AsRecordType());
			r->Assign(0, new Val(return_list[i]->_value, TYPE_DOUBLE));
			r->Assign(2, new Val(return_list[i]->_aid, TYPE_ENUM));
			r->Assign(1, new Val(return_list[i]->_tag, TYPE_ENUM));
			(*vl)[1]->AsVectorVal()->Assign(i, r);
		}
				//RecordVal* tmp_id = _current_conn_id->AsRecordVal();
		vl->append(_current_conn_id->Clone());
		mgr.QueueEvent(FeatureAnalysis::metric_event, vl);
	}
}

// extract the value from the vector based on the analysis id and input tag
Val* plugin::Analysis_FeatureAnalysis::Plugin::Extract(Val* v, Val* aid, Val* tag)
{
	unsigned int i;
	unsigned int n = v->AsVectorVal()->Size();
	for (i = 0; i < n; i++)
	{
		RecordVal* e = (v->AsVectorVal())->Lookup(i)->AsRecordVal();
		if ((e->Lookup("tag")->AsEnum() == tag->AsEnum()) && 
				(e->Lookup("aid")->AsEnum() == aid->AsEnum())) 
			return new Val(e->Lookup("value")->AsDouble(), TYPE_DOUBLE);
	}
	return new Val(-DBL_MAX, TYPE_DOUBLE);
}

// strip all other information and just represent the metric vector
Val* plugin::Analysis_FeatureAnalysis::Plugin::ExtractVector(Val* v)
{
	unsigned int i;
	unsigned int n = v->AsVectorVal()->Size();
	VectorVal* rv = new VectorVal(internal_type("FeatureAnalysis::feature_vector")->AsVectorType());
	for (i = 0; i < n; i++)
	{
		RecordVal* e = (v->AsVectorVal())->Lookup(i)->AsRecordVal();
		rv->Assign(i, new Val(e->Lookup("value")->AsDouble(), TYPE_DOUBLE));
	}
	return rv;
}
