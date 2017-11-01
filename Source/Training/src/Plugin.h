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
* Plugin.h : Implements bin.bif. Internally hold a dictionary of training data  *
*       identified by flow UID. Each incoming feature value is passed directly  *
*       to its associated IntervalTraining object.                              *
* Content :                                                                     *
*   * RemoveConnection                                                          *
*       Remove the connection from the dictionary and print the outputs if      *
*       the conditions are fulfilled                                            *
*   * add_feature                                                               *
*       Add the data point both to individual flows and aggregate flow (ALL)    *
*   * SetBinCount                                                               *
*       Set the bin numbers                                                     *
*   * SetKSDataCount                                                            *
*       Set the data number for the normal data sizes and how many normal data  *
*   * ChangePrefix                                                              *
*       Change the prefix to the output file                                    *
\*******************************************************************************/

#ifndef BRO_PLUGIN_TRAINING_BIN
#define BRO_PLUGIN_TRAINING_BIN

#include <plugin/Plugin.h>
#include "Bin_Training.h"
#include <vector>
#include <unordered_map>
#include <memory>

#define DEFAULT_TAG_COUNT 3
#define KS_TRAINING_DATA_NUMBER 100
#define KS_TRAINING_DATA_THRESHOLD 1000
#define BIN_TRAINING_DATA_THRESHOLD 1000
#define DEFAULT_BIN_COUNT 5

namespace plugin {
namespace Training_Bin {

class Plugin : public ::plugin::Plugin
{
public:
	void RemoveConnection(StringVal* UID, Val* direction, Val* tag);
	void add_feature(StringVal* UID, Val* direction, double feature, Val* tag_val);
    void SetBinCount(Val* bin_counts);
    void SetKSDataCount(Val* bin_counts, int max_counter);
    void ChangePrefix(StringVal* prefix);
    void ChangeThreshold(int min_KS, int min_bin);
protected:
	// Overridden from plugin::Plugin.
private:
    std::shared_ptr<IntervalTraining> getFlow(std::string UID_str, int direction, int tag);

	std::unordered_map <std::string, std::shared_ptr<BiFlow> > _flow_dict;
	virtual plugin::Configuration Configure();
	unsigned int _tag_count;
	std::vector<unsigned int> _bin_counts;
    std::vector<unsigned int> _ks_data_count;
    unsigned int _ks_max_counter;
    unsigned int _ks_counter;
    unsigned int _ks_threshold;
    unsigned int _bin_threshold;
    std::string _prefix;
};

extern Plugin plugin;

}
}

#endif
