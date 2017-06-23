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

#ifndef BRO_PLUGIN_ANALYSIS_FEATUREANALYSIS_H
#define BRO_PLUGIN_ANALYSIS_FEATUREANALYSIS_H

#include <plugin/Plugin.h>
#include <unordered_map>		// unordered_map
#include <vector>			// vector
#include <memory>			// shared_ptr
#include "Flow.h"			// Flow
#include "Bin_Strategy.h"		// Bin allocation
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
#define DEFAULT_KS_WINDOW_SIZE 10
#define DEFAULT_REGULARITY_WINDOW_SIZE 10
#define DEFAULT_REGULARITY_WINDOW_NUMBER 1000
#define DEFAULT_CCE_PATTERN_SIZE 5


class Plugin : public ::plugin::Plugin
{
public:
	void RemoveConnection(StringVal* UID);

	void RegisterAnalysis(StringVal* UID, Val* Set_ID, Val* conn_ID);
	void AddFeature(StringVal* UID, double feature, Val* aid_val, Val* tag_val);
	void CalculateMetric();

	void SetStepSize(Val* Set_ID, unsigned int step_size);
	Val* Extract(Val* v, Val* aid, Val* tag);
	Val* ExtractVector(Val* v);
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
private:
	// map from UID to Flow object
	std::unordered_map <std::string, std::shared_ptr<CCD::Flow> > _flow_dict;
	std::shared_ptr<CCD::FlowConfig> _flow_config;	// global config for all flows
	// temporary holder is useful so that we don't have to do the lookup multiple times for the same flow.
	std::shared_ptr<CCD::Flow> _current_flow; 	// temporary holder
	std::string* _current_UID;			// temporary holder's UID
	unsigned int _current_set_ID;			// temporary holder's set_ID
	Val* _current_conn_id;				// temporary holder's 4 network tuple
};

extern Plugin plugin;

}
}

#endif
