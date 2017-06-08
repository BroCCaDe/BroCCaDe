#include <vector>
#include <memory>
#include "Flow.h"
#include "Histogram.h"
#include "Data_Container.h"
#include "Pattern_Data_Container.h"
#include "Regularity_Data_Container.h"
#include "KS_Flow.h"
#include "Entropy_Flow.h"
#include "CCE_Flow.h"
#include "MultiModal_Flow.h"
#include "Autocorrelation_Flow.h"
#include "Regularity_Flow.h"

//#define DEBUG_H

#ifdef DEBUG_H
#include <stdio.h>
#endif

// assume : idx < size
#define BOUND_CHECK_RESIZE(v, idx1, idx2, idx3, size1, size2, size3) \
	if (v.size() <= idx1) v.resize(size1); \
	if (v[idx1].size() <= idx2) v[idx1].resize(size2); \
	if (v[idx1][idx2].size() <= idx3) v[idx1][idx2].resize(size3);

using namespace CCD;

/*
FlowAnalyzer::FlowAnalyzer(std::unique_ptr<Data_Container> data, unsigned int step_size)
{
	_data.swap(data);
	_step_size = step_size;
	_step = 0;
}

// Check if the metric calculation should be triggered after adding a feature value
bool FlowAnalyzer::add_feature(double feature, double* metric) 
{
	_data->add_feature(feature);

#ifdef DEBUG_H
	printf("Flow.cc: currently step %d\n", _step);
#endif
	
	// if the metric calculation is triggered, reset the step
	if (++_step >= _step_size)
	{
		*metric = calculate_metric();
		_step = 0;
		return true;
	}
	// the metric calculation is not yet triggered
	return false;
}
*/

void Flow::begin_adding_feature(unsigned int set_ID)
{
	_current_set_ID = set_ID;
	_result.clear();
	_step++;
}

void Flow::add_feature(unsigned int tag, unsigned int aid, double feature)
{
	// sanity check: the current set ID must be set, i.e. begin_adding_feature has been called
	if (_current_set_ID == -1) return;

	Data_Container_Enum type = _config->map_analysis_data[aid];

	// it is assumed that the config has the highest possible values
	BOUND_CHECK_RESIZE(_data, (unsigned int) _current_set_ID, tag, type,
		_config->set_IDs, _config->tag_count, _config->type_count);

//	printf ("feature: type %d - data %d\n", aid, type);

	if (_data[_current_set_ID][tag][type].get() == NULL)
	{
		// construct new data container
		switch (type)
		{
			case RAW_DATA :
			{
				_data[_current_set_ID][tag][type] = std::shared_ptr<Raw_Data>
					(new Raw_Data (_config->KS_window_size, 
					_config->step_sizes[_current_set_ID]));
				break;	
			}
			case HISTOGRAM_DATA :
			{
				_data[_current_set_ID][tag][type] = std::shared_ptr<Histogram>
					(new Histogram (_config->binner));
				break;	
			}
			case PATTERN_DATA :
			{
				_data[_current_set_ID][tag][type] = 
					std::shared_ptr<Pattern_Data> (new Pattern_Data (
					_config->CCE_pattern_size, _config->binner));
				break;
			}
			case REGULARITY_DATA :
			{
				_data[_current_set_ID][tag][type] = 
					std::shared_ptr<Regularity_Data> (new Regularity_Data (
						_config->Regularity_window_number,
						_config->Regularity_window_size));
				break;
			}
			case NULL_DATA :
			{
				_data[_current_set_ID][tag][type] = 
					std::shared_ptr<Null_Data> (new Null_Data());
				break;
			}
		}
	}
	_data[_current_set_ID][tag][type]->add_feature(feature);
	if (_step >= _config->step_sizes[_current_set_ID]) add_analysis(tag, aid);
}

void Flow::add_analysis(unsigned int tag, unsigned int aid)
{
	// it is assumed that the config has the highest possible values
	BOUND_CHECK_RESIZE(_analysis, (unsigned int) _current_set_ID, tag, aid, 
		_config->set_IDs, _config->tag_count, _config->number_of_analysis);

	if (_analysis[_current_set_ID][tag][aid].get() == NULL)
	{
		Data_Container_Enum type = _config->map_analysis_data[aid];
//		printf ("analysis: type %d - data %d\n", aid, type);
		// otherwise create a new analysis object and attach it to the flow
		if (aid == _config->KS_analysis)
			_analysis[_current_set_ID][tag][aid] = std::shared_ptr<KS> (new KS 
				(std::static_pointer_cast<Raw_Data>
				(_data[_current_set_ID][tag][type]), _config->KS_normal_data));			
		else if (aid == _config->Entropy_analysis)
			_analysis[_current_set_ID][tag][aid] = std::shared_ptr<Entropy>
				(new Entropy(std::static_pointer_cast<Histogram>
				(_data[_current_set_ID][tag][type])));	
		else if (aid == _config->CCE_analysis)
			_analysis[_current_set_ID][tag][aid] = std::shared_ptr<CCE>
				(new CCE(std::static_pointer_cast<Pattern_Data>
				(_data[_current_set_ID][tag][type])));
		else if (aid == _config->MultiModal_analysis)
				_analysis[_current_set_ID][tag][aid] = std::shared_ptr<MultiModal>
				(new MultiModal(std::static_pointer_cast<Histogram>
				(_data[_current_set_ID][tag][type])));
		else if (aid == _config->Autocorrelation_analysis)
			_analysis[_current_set_ID][tag][aid] = std::shared_ptr<Autocorrelation>
				(new Autocorrelation(std::static_pointer_cast<Raw_Data>
				(_data[_current_set_ID][tag][type]),	_config->Autocorrelation_lags));
		else if (aid == _config->Regularity_analysis)
			_analysis[_current_set_ID][tag][aid] = std::shared_ptr<Regularity>
				(new Regularity(std::static_pointer_cast<Regularity_Data>
				(_data [_current_set_ID][tag][type])));
		else if (aid == _config->Null_analysis)
			_analysis[_current_set_ID][tag][aid] = std::shared_ptr<NullAnalysis>
				(new NullAnalysis(std::static_pointer_cast<Null_Data>
				(_data[_current_set_ID][tag][type])));
/*
		switch (aid)
		{
			case KS_ANALYSIS : 
			{
				_analysis[_current_set_ID][tag][aid] = std::shared_ptr<KS>
					(new KS (_analysis[_current_set_ID][tag][aid], _config->KS_normal_data));
				break;
			}
			case ENTROPY_ANALYSIS : 
			{
				_analysis[_current_set_ID][tag][aid] = std::shared_ptr<Entropy>
					(new Entropy(_analysis[_current_set_ID][tag][aid]));
				break;
			}
			case CCE_ANALYSIS : 
			{
				_analysis[_current_set_ID][tag][aid] = std::shared_ptr<CCE>
					(new CCE(_analysis[_current_set_ID][tag][aid]));
				break;
			}
			case MULTIMODAL_ANALYSIS :
			{
				_analysis[_current_set_ID][tag][aid] = std::shared_ptr<MultiModal>
					(new MultiModal(_analysis[_current_set_ID][tag][aid]));
				break;
			}
			case AUTOCORRELATION_ANALYSIS :
			{
				_analysis[_current_set_ID][tag][aid] = std::shared_ptr<Autocorrelation>
					(new Autocorrelation(_analysis[_current_set_ID][tag][aid], 
					_config->Autocorrelation_lags));
				break;
			}
			case NULL_ANALYSIS :
			{
				_analysis[_current_set_ID][tag][aid] = std::shared_ptr<NullAnalysis>
					(new NullAnalysis(_analysis[_current_set_ID][tag][aid]));
			}
			case REGULARITY_ANALYSIS :
			{
				_analysis[_current_set_ID][tag][aid] = std::shared_ptr<Regularity>
					(new Regularity(_analysis [_current_set_ID][tag][aid]));
			}
			default : break;
		}
*/
	}

	_result.push_back(std::unique_ptr<TempValue>(new TempValue(
		_analysis[_current_set_ID][tag][aid]->calculate_metric(), aid, tag)));
}

bool Flow::end_adding_feature()
{
	if (_step >= _config->step_sizes[_current_set_ID]) 
	{
		_current_set_ID = -1;
		_step = 0;
		return true;
	}
	_current_set_ID = -1;
	return false;
}

std::vector<std::unique_ptr<TempValue>> Flow::get_result()
{
	std::vector<std::unique_ptr<TempValue> > result;
	result.swap(_result);
	return result;
}
