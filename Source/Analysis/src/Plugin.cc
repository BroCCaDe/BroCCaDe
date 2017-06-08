#include "Plugin.h"
#include "Flow.h"
/*
#include "KS_Flow.h"
#include "Entropy_Flow.h"
#include "CCE_Flow.h"
#include "MultiModal_Flow.h"
#include "Autocorrelation_Flow.h"
*/
#include "analysis.bif.h"
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

/*
	// statically create the null analysis class which is used for every
	// null analysis request
	std::shared_ptr<NullAnalysis> tmp_null_analysis (new NullAnalysis());
	_null_analysis.swap(tmp_null_analysis);
*/

	// initialize the config data for the flow analysis
	_flow_config = std::shared_ptr<FlowConfig> (new FlowConfig());
	_flow_config->binner = std::shared_ptr<Bin_Strategy_Interval> 
		(new Bin_Strategy_Interval());
	_flow_config->KS_normal_data = std::shared_ptr<std::vector<double> > 
		(new std::vector<double>());
	_flow_config->Autocorrelation_lags = std::shared_ptr<
		std::vector<unsigned int> > (new std::vector<unsigned int>());

	_flow_config->KS_window_size = 10;
	_flow_config->Regularity_window_number = 1000;
	_flow_config->Regularity_window_size = 10;
	_flow_config->CCE_pattern_size = 5;
	_flow_config->tag_count = DEFAULT_TAG_COUNT;
	_flow_config->step_sizes.resize(DEFAULT_SET_IDS);
	_flow_config->step_sizes[0] = 1; 		// hard coded for now
	_flow_config->step_sizes[1] = DEFAULT_STEP_SIZE;// hard coded for now
	_flow_config->step_sizes[2] = 1; 		// hard coded for now

	_flow_config->set_IDs = DEFAULT_SET_IDS;
	_flow_config->type_count = CCD::DATA_CONTAINER_TYPE_COUNT;
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

/*
// register the request for analysis which is distinguished by its analysis ID and tag
void plugin::Analysis_FeatureAnalysis::Plugin::RegisterAnalysis(StringVal* UID, double feature,
	Val* aid_val, Val* tag_val)
{
	//BroString* newStr = new BroString(*(UID->AsString()));
	std::string *UID_str = new std::string((const char*) UID->Bytes());	
	Analysis_ID aid = (Analysis_ID) aid_val->AsEnum();
	int tag = tag_val->AsEnum();

	// sanity check for aid
	if (aid < 0 || aid > _config->number_of_analysis)
	{
		printf("error: invalid analysis ID %d\n", aid);
		return;
	}

	// sanity check for tag
	if (tag < 0)
	{
		printf("error: invalid Tag %d\n", tag);
		return;
	}

	if ((unsigned int) tag >= _config->tag_count) {_config->_tag_count = (unsigned int) tag + 1;}

	_request_list.push_back(std::unique_ptr<RequestValue> 
		(new RequestValue(UID_str, feature, aid, tag)));
}*/

// register the request for analysis which is distinguished by its analysis ID and tag
void plugin::Analysis_FeatureAnalysis::Plugin::RegisterAnalysis(StringVal* UID, Val* Set_ID, Val* conn_ID)
{
	//BroString* newStr = new BroString(*(UID->AsString()));
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

	delete(_current_UID);
	delete(_current_conn_id);

	_current_flow = flow;
	_current_UID = UID_str;
	_current_conn_id = conn_ID->Clone();
	_current_set_ID = set_ID;

	flow->begin_adding_feature(set_ID);
}

// register the request for analysis which is distinguished by its analysis ID and tag
void plugin::Analysis_FeatureAnalysis::Plugin::AddFeature(StringVal* UID, double feature,
	Val* aid_val, Val* tag_val)
{
	//BroString* newStr = new BroString(*(UID->AsString()));
	std::string *UID_str = new std::string((const char*) UID->Bytes());	
	if (UID_str->compare(*_current_UID))
	{
		printf("error: current UID is not the same as the input UID\n");
		return;
	}
	delete(UID_str);

	Analysis_ID aid = (Analysis_ID) aid_val->AsEnum();
	int tag = tag_val->AsEnum();

	// sanity check for aid
	if (aid < 0 || aid > _flow_config->number_of_analysis)
	{
		printf("error: invalid analysis ID %d\n", aid);
		return;
	}

	// sanity check for tag
	if (tag < 0)
	{
		printf("error: invalid Tag %d\n", tag);
		return;
	}

	if ((unsigned int) tag >= _flow_config->tag_count) {_flow_config->tag_count = (unsigned int) tag + 1;}

	_current_flow->add_feature(tag, aid, feature);
}


// Act upon the analysis request
void plugin::Analysis_FeatureAnalysis::Plugin::CalculateMetric()
//void plugin::Analysis_FeatureAnalysis::Plugin::CalculateMetric(Val* Set_ID)
{
/*	if (_request_list.size() <= 0) return; // just break if there is no request
	unsigned int index;
	int set_ID = Set_ID->AsEnum();
	if ((unsigned int) set_ID >= _set_IDs) {_config->_set_IDs = (unsigned int) set_ID + 1;}
	std::vector<std::unique_ptr<TempValue>> return_list;
	return_list.clear();

	// XXX: At the moment we assume that the UID will always be the same
	//  taken from the first request. This way we do not have to lookup the map
	//  over and over again
	std::string *UID_str = _request_list[0]->_UID;


	std::unordered_map <std::string, std::shared_ptr< std::vector< std::vector< 
		std::vector< std::shared_ptr<FeatureAnalyzer> > > > > >::iterator 
			got = _flow_dict.find(*UID_str);

	std::shared_ptr<std::vector< std::vector< std::vector <
		std::shared_ptr<FeatureAnalyzer> > > >> analysis;
	// check if we have the flow in the dictionary
	if (got == _flow_dict.end())
	{
#ifdef DEBUG_H
		printf("creating new flow ID: %s\n", UID_str->c_str());
#endif
		// if not then create a new entry
		analysis = std::shared_ptr< std::vector< std::vector< 
			std::vector< std::shared_ptr<FeatureAnalyzer>> > > >
			(new std::vector< std::vector< 
			std::vector< std::shared_ptr<FeatureAnalyzer>> > >());
		pair<std::unordered_map<std::string, std::shared_ptr< std::vector< std::vector< 
			std::vector< std::shared_ptr<FeatureAnalyzer>> > > > >::iterator, bool>
			result = _flow_dict.emplace(*UID_str, analysis);
		analysis->resize(_set_IDs);
		
		// and check that the entry is created successfully
		if (!result.second) 
			printf("error in creating new analysis per flow\n");
	}
	else
	{ 
		analysis = got->second;
#ifdef DEBUG_H
		printf("fetching old flow where size is %lu\n", analysis->size());
#endif
		// dynamically adjust the number of analysis set
		if (analysis->size() < _set_IDs)
			analysis->resize(_set_IDs);
	}

	// iterate over all the analysis requests
	for(index = 0; index < _request_list.size(); index++)
	{
		Analysis_ID aid = _request_list[index]->_aid;
		int tag = _request_list[index]->_tag;
		double feature = _request_list[index]->_value;
	

		// this is needed to dynamically adjust discrepancy of the number of tags
		if (ACCESS_1(analysis, set_ID).size() < _tag_count)		
		{
			ACCESS_1(analysis, set_ID).resize(_tag_count);
		}

		// set the vector to the number of analysis
		if (ACCESS_2(analysis, set_ID, tag).size() < NUMBER_OF_ANALYSIS)
		{
			ACCESS_2(analysis, set_ID, tag).resize(NUMBER_OF_ANALYSIS);
		}

		std::shared_ptr<FeatureAnalyzer> a = _null_analysis;

		// check if the analysis ID exists
		if (ACCESS_3(analysis, set_ID, tag, aid).get() == NULL)
		{
			// otherwise create a new analysis object and attach it to the flow
			switch (aid)
			{
				case KS_ANALYSIS : 
				{
					ACCESS_3(analysis, set_ID, tag, aid) = std::shared_ptr<KS>
						(new KS(10, _step_size, _normal_data));
					a = ACCESS_3(analysis, set_ID, tag, aid);
					break;
				}
				case ENTROPY_ANALYSIS : 
				{
					ACCESS_3(analysis, set_ID, tag, aid) = std::shared_ptr<Entropy>
						(new Entropy(_step_size, _binner));
					a = ACCESS_3(analysis, set_ID, tag, aid);
					break;
				}
				case CCE_ANALYSIS : 
				{
					ACCESS_3(analysis, set_ID, tag, aid) = std::shared_ptr<CCE>
						(new CCE(_step_size, 5, _binner));
					a = ACCESS_3(analysis, set_ID, tag, aid);
					break;
				}
				case MULTIMODAL_ANALYSIS :
				{
					ACCESS_3(analysis, set_ID, tag, aid) = std::shared_ptr<MultiModal>
						(new MultiModal(_step_size, _binner));
					a = ACCESS_3(analysis, set_ID, tag, aid);
					break;
				}
				case AUTOCORRELATION_ANALYSIS :
				{
					ACCESS_3(analysis, set_ID, tag, aid) = std::shared_ptr<Autocorrelation>
						(new Autocorrelation(10, _step_size));
					a = ACCESS_3(analysis, set_ID, tag, aid);
					break;
				}
				default : break;
			}
		}
		else
		{
			a = ACCESS_3(analysis, set_ID, tag, aid);
		}
		
		double metric;
		bool metric_calculated = a->add_feature(feature, &metric);
	
		if (metric_calculated)
		{
#ifdef DEBUG_H
			printf("(%d:%d) metric is %lf\n", aid, tag, metric);
#endif
			return_list.push_back(std::unique_ptr<TempValue>(new TempValue(metric, aid, tag)));
		} 
	}
*/
	unsigned int index;
	bool metric_calculated = _current_flow->end_adding_feature();

	// send the actual event
	if (metric_calculated) 
	{
		std::vector<std::unique_ptr<CCD::TempValue>> return_list = _current_flow->get_result();

		val_list* vl = new val_list;
		vl->append(new Val(_current_set_ID, TYPE_ENUM));
		vl->append(new VectorVal(internal_type("result_vector")->AsVectorType()));
		unsigned long list_size = return_list.size();
	
		unsigned long i;
		for (i = 0; i < list_size; i++)
		{
			RecordVal* r = new RecordVal(internal_type("analysis_result")->AsRecordType());
			r->Assign(0, new Val(return_list[i]->_value, TYPE_DOUBLE));
			r->Assign(1, new Val(return_list[i]->_aid, TYPE_ENUM));
			r->Assign(2, new Val(return_list[i]->_tag, TYPE_ENUM));
			(*vl)[1]->AsVectorVal()->Assign(i, r);
		}
		mgr.QueueEvent(FeatureAnalysis::metric_event, vl);
	}

/*	_request_list.clear();
	return_list.clear();
*/
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
	//VectorVal* rv = new VectorVal(new VectorType(base_type(TYPE_DOUBLE)));
	VectorVal* rv = new VectorVal(internal_type("FeatureAnalysis::feature_vector")->AsVectorType());
	for (i = 0; i < n; i++)
	{
		RecordVal* e = (v->AsVectorVal())->Lookup(i)->AsRecordVal();
		rv->Assign(i, new Val(e->Lookup("value")->AsDouble(), TYPE_DOUBLE));
	}
	return rv;
}
