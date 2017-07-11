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
*                                                                               *
* Flow.h : the core of the extension, maintain information needed for each flow,*
*       both the data containers and the analysis engines.                      *
* Contents:                                                                     *
*   * class FlowConfig : this class is used to hold config information shared   *
*           across all flows.                                                   *
*   * class Flow : triggers metric calculation when a certain number of features*
*           are added to the flow. The process of adding feature is             *
*           transactional, i.e. there are clear markings of beginning and end.  *
*           Currently it is assumed that the transaction will go without problem*
*           so there will be undefined behaviour when a problem occured         *
*   * class TempValue : this is a data structure that holds the outstanding     *
*           metric values calculated by the Flow class, distinguished by the    *
*           analysis id and analysis tag.                                       *
\*******************************************************************************/

#ifndef AUX_PLUGINS_FLOW_H
#define AUX_PLUGINS_FLOW_H

#include <vector>           // vector
#include <memory>           // unique_ptr
#include "Data_Container.h" // Data_Container
#include "Analysis.h"       // superclass
#include "Bin_Strategy.h"   // bin allocation

namespace CCD {

class TempValue;

// global config for all flows. This should be set-up in the plugin before creating any Flow
class FlowConfig {
public:
	FlowConfig() {}
	unsigned int KS_window_size;			// window size for KS raw data
	unsigned int Regularity_window_number;  // number of window for regularity metric
	unsigned int Regularity_window_size;    // the size of each window for regularity metric
	unsigned int CCE_pattern_size;          // the length of pattern for CCE
	std::vector<unsigned int> step_sizes;   // step size for each calculation set
	unsigned int tag_count;                 // maximum number of analysis tags (dynamic)
	unsigned int type_count;                // maximum number of data container type (static)
	unsigned int set_IDs;                   // maximum known value of set_ID
	unsigned int number_of_analysis;        // maximum number of analysis (static)
	std::vector<Data_Container_Enum>        // map from analysis ID to data container type
		map_analysis_data;                  //   (static)

	// these ID should be treated as an enum, i.e. a static mapping from the analysis_ID enum in 
	// .bif file to each analysis.
	unsigned int KS_analysis;               // analysis ID for KS test
	unsigned int Entropy_analysis;          // analysis ID for Entropy
	unsigned int CCE_analysis;              // analysis ID for CCE
	unsigned int Regularity_analysis;       // analysis ID for Regularity
	unsigned int Autocorrelation_analysis;  // analysis ID for Autocorrelation
	unsigned int MultiModal_analysis;       // analysis ID for Multi-Modality
	unsigned int Null_analysis;             // analysis ID for null analysis

	// lags for autocorrelation calculation
	std::shared_ptr<std::vector<unsigned int> > Autocorrelation_lags;
	// normal data to test against. Used for KS test
	std::vector<std::shared_ptr<std::vector<double> > > KS_normal_data;
	std::vector<std::vector<std::shared_ptr<Bin_Strategy> >> binner;
    std::shared_ptr<Bin_Strategy> default_binner;
    // static Null_Data_Container and Null_Analysis
    std::shared_ptr<Null_Data> null_data;
    std::shared_ptr<NullAnalysis> null_analysis;
};

// the class that holds the data container and analysis per flow.
// Currently we store the analysis objects so that we don't have to create and destroy the analysis
// object for each analysis triggered.
// Each calculation set is clearly distinguished by the begin - end method call.
// The adding feature in between the begin - end pair is assumed to be in the context specified
// in the begin_adding_feature method. If the calculation is triggered, then the add_analysis
// method is also invoked.
class Flow {
public:
	Flow (std::shared_ptr<FlowConfig> config)
	{
		_data.clear();                                  // initialize the map to data containers
		_data.resize(config->set_IDs);
		_analysis.clear();                              // initialize the map to the analysis
		_analysis.resize(config->set_IDs);
		_reset.clear();
		_steps.resize(config->set_IDs, 0);              // initialize the steps for each calculation set
		_current_set_ID = -1;			
		_config = config;                               // copy the reference to the config
	}

	void begin_adding_feature(unsigned int set_ID);     // begin
	// adding feature method identified by tag and list of analysis IDs.
	// The list of analysis will share the feature value, and the feature value will be added
	// to each analysis' data container (no duplicate)
	void add_feature(unsigned int tag, std::vector<unsigned int> aid, double feature);
	bool end_adding_feature();                          // end

	std::vector<std::unique_ptr<TempValue> > get_result();
private:
	// trigger the analysis identified by the tag and analysis ID.
	// This should be called after we insert the data in add_feature
	void add_analysis(unsigned int tag, unsigned int aid);

	// map from (set_ID, tag, aid) to a data container
	std::vector< std::vector< std::vector <
		std::shared_ptr <Data_Container> > > > _data;
	// map from (set_ID, tag, aid) to an analysis object
	std::vector< std::vector< std::vector <
		std::shared_ptr <FeatureAnalyzer> > > > _analysis;
	std::vector<std::shared_ptr <Raw_Data> > _reset;    // raw data which needs window reset
	std::shared_ptr<FlowConfig> _config;                // local reference to the global flow config
	std::vector<unsigned int> _steps;                   // steps for each calculation set
	std::vector<std::unique_ptr<TempValue> > _result;
	int _current_set_ID;                                // current calculation set ID
};

class BiFlow {
public:
    BiFlow(std::shared_ptr<FlowConfig> config) : _config(config) {_flows.resize(2);}
    std::shared_ptr<Flow> get(unsigned int direction) {
        if (direction < _flows.size()) 
        {
            if (_flows[direction].get() == NULL)
                _flows[direction] = std::shared_ptr<Flow> (new Flow(_config));
            return _flows[direction];
        }
        printf("weird direction : %u\n", direction);
        exit (1);
    }
private:
    std::vector<std::shared_ptr<Flow> > _flows;
    std::shared_ptr<FlowConfig> _config;
};

// used to store the value of the calculated metric with its accompanying analysis ID and input tag
class TempValue {
public:
	TempValue(double value, int aid, int tag) : _value(value), _aid(aid), _tag(tag) {}
	~TempValue() {}
	double _value;
	int _aid;
	int _tag;
};

}

#endif
