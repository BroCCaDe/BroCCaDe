/*******************************************************************************\
* Copyright (c) 2017 Murdoch University                                         *
*                                                                               *
* Written by Hendra Gunadi (Hendra.Gunadi@murdoch.edu.au)                       *
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
* Plugin.h : Bro plugin with internal dictionary of flows. The main function of *
*       of the plugin is to lookup flow of interest and interface it with the   *
*       Bro script. It implements all the functions specified in analysis.bif.h *
* Content:                                                                      *
*   * GetDirection                                                              *
*       Determine the packet direction by comparing the value of packet source  *
*       and the source of the Bro connection record                             *
*   * ConfigureInternalType                                                     *
*       Tell the plugin to cache Bro type internally to avoid the overhead of   *
*       looking up the type each time.                                          *
*   * Extract                                                                   *
*       extract a value from vector of metric result                            *
*   * Extract_vector                                                            *
*       extract vector of double values from the vector of metric result        *
*   * SetStepSize                                                               *
*       set the number of data to trigger metric calculation                    *
*   * Set_KS_Window_Size                                                        *
*       Set the maximum size of window for KS and Autocorrelation metrics       *
*   * Set_Regularity_Parameters                                                 *
*       Set the window number and the window size for regularity metric         *
*   * Set_CCE_Pattern_Size                                                      *
*       Set the depth of CCE tree                                               *
*   * Set_Autocorrelation_Lags                                                  *
*       Set the number of autocorrelation lags (1 .. n)                         *
*   * Add_Normal_Data                                                           *
*       Add a vector of values (from file) which constitutes the normal data to *
*       compare against in the KS metric calculation                            *
*   * Load_Interval                                                             *
*       Set the equiprobable bin interval for CCE                               *
*   * Set_Bin_Null                                                              *
*       Set the fine grained bin interval (each value as its own bin) for TTL   *
*       and Packet length features, Entropy and Multi Modality metrics          * 
*   * RemoveConnection                                                          *
*       remove a connection from the dictionary                                 *
*   * RegisterAnalysis                                                          *
*       indicate the beginning of a transaction to add feature                  *
*   * AddFeature                                                                *
*       add the feature associated with the analyzers                           *
*   * CalculateMetric                                                           *
*       indicate the end of a transaction to add feature                        *
\*******************************************************************************/

#ifndef BRO_PLUGIN_ANALYSIS_FEATUREANALYSIS_H
#define BRO_PLUGIN_ANALYSIS_FEATUREANALYSIS_H

#include <plugin/Plugin.h>
#include <unordered_map>        // unordered_map
#include <vector>               // vector
#include <memory>               // shared_ptr
#include "Flow.h"               // Flow
#include "Bin_Strategy.h"       // Bin allocation
#include "analysis.bif.h"

namespace plugin {
namespace Analysis_FeatureAnalysis {

using namespace BifEnum::FeatureAnalysis;

// NUMBER_OF_ANALYSIS must match the enum defined in analysis.bif
#define NUMBER_OF_ANALYSIS 7
// the following constants are arbitrary, as long as it is of reasonable integer value
#define DEFAULT_TAG_COUNT 3
#define DEFAULT_STEP_SIZE 10
#define DEFAULT_SET_IDS 3
#define DEFAULT_KS_WINDOW_SIZE 100
#define DEFAULT_REGULARITY_WINDOW_SIZE 100
#define DEFAULT_REGULARITY_WINDOW_NUMBER 10
#define DEFAULT_CCE_PATTERN_SIZE 5

class Plugin : public ::plugin::Plugin
{
public:
	void Add_Normal_Data(Val* tag_val, StringVal* KS_data_file);
    void Load_Interval(Val* tag_val, Val* aid_val, StringVal* Interval_data_file);
    void Set_Bin_Null(Val* tag_val, Val* aid_val, unsigned int bin_count);
	void RemoveConnection(StringVal* UID);
    void Set_KS_Window_Size(Val* Set_ID, unsigned int value);
    void Set_Regularity_Parameters(Val* Set_ID, unsigned int window_number, unsigned int window_size);
    void Set_CCE_Pattern_Size(Val* Set_ID, unsigned int value);
    void Set_Autocorrelation_Lags(Val* Set_ID, unsigned int lag_max);

	void RegisterAnalysis(StringVal* UID, Val* Set_ID, Val* conn_ID, Val* direction_val);
	void AddFeature(double feature, Val* aid_val, Val* tag_val);
	void CalculateMetric();

    void ConfigureInternalType();
	void SetStepSize(Val* Set_ID, unsigned int step_size);
    EnumVal* GetDirection(Val* conn_source, Val* pkt_source);
	Val* Extract(Val* v, Val* aid, Val* tag);
	Val* ExtractVector(Val* v);
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
private:
	// map from UID to Flow object
	std::unordered_map <std::string, std::shared_ptr<CCD::BiFlow> > _flow_dict;
	std::shared_ptr<CCD::FlowConfig> _flow_config;  // global config for all flows
	// temporary holder is useful so that we don't have to do the lookup multiple times for the same flow.
	std::shared_ptr<CCD::Flow> _current_flow;       // temporary holder for 5 tuple ID
    std::unique_ptr<IPAddr> _current_src_ip;
    std::unique_ptr<IPAddr> _current_dst_ip;
    uint32 _current_src_port;
    uint32 _current_dst_port;
    TransportProto _current_src_proto;
    TransportProto _current_dst_proto;
	unsigned int _current_set_ID;			        // temporary holder's set_ID
    unsigned int _current_direction;                // temporary holder's direction

    // cache for Bro type
    VectorType* result_vector_type;
    RecordType* analysis_result_type;
    VectorType* feature_vector_type;
    EnumType* direction_enum_type;
};

extern Plugin plugin;

}
}

#endif
