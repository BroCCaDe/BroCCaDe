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
#include <iostream>
#include <fstream>

#define ACCESS(arr, idx_1) (*arr)[idx_1]

namespace plugin { namespace Training_Bin { Plugin plugin; } }

using namespace plugin::Training_Bin;

plugin::Configuration Plugin::Configure()
	{
	plugin::Configuration config;
	config.name = "Training::Bin";
	config.description = "<Insert description>";
	config.version.major = 0;
	config.version.minor = 1;

    _ks_threshold = KS_TRAINING_DATA_THRESHOLD;
    _bin_threshold = BIN_TRAINING_DATA_THRESHOLD;
	_tag_count = DEFAULT_TAG_COUNT;
	_bin_counts.push_back(DEFAULT_BIN_COUNT);
    _ks_data_count.push_back(KS_TRAINING_DATA_NUMBER);
    _prefix = "";
    _ks_max_counter = 0;
    _ks_counter = 0;

	return config;
	}

void print_pairs(std::vector< std::pair<double, double> > pairs, std::string file_name)
{
	if (pairs.size() <= 0) return;
	std::vector< std::pair<double, double>>::const_iterator it;
	ofstream f;
	f.open(file_name);
	if (!f.is_open()) {printf("(pairs) can't open file\n"); return;}
	f << pairs.size() << "\n";
	for (it = pairs.begin(); it != pairs.end(); it++)
	{
		f << it->first << " " << it->second << "\n";
	}
	f.close();
}

void print_data(std::vector<double> data, std::string file_name, unsigned int count)
{
	if (data.size() <= 0) return;
	std::vector<double>::const_iterator it;
	ofstream f;
  	f.open (file_name);
	if (!f.is_open()) {printf("(data) can't open file\n"); return;}
	f << count << "\n";
	for (it = data.begin(); it != data.begin()+count; it++) f << *it << "\n"; 
	f.close();
}

void plugin::Training_Bin::Plugin::ChangeThreshold(int min_KS, int min_bin) {
    _ks_threshold = min_KS;
    _bin_threshold = min_bin;
}

void plugin::Training_Bin::Plugin::SetBinCount(Val* bin_counts)
{
    _bin_counts.clear();
    std::vector<Val*>* bin_count_vector = bin_counts->AsVector();
    for (std::vector<Val*>::iterator it = bin_count_vector->begin();
        it != bin_count_vector->end(); it++) {
        _bin_counts.push_back((*it)->AsCount());
    }
}

void plugin::Training_Bin::Plugin::SetKSDataCount(Val* data_counts, int max_counter)
{
    _ks_data_count.clear();
    std::vector<Val*>* data_count_vector = data_counts->AsVector();
    for (std::vector<Val*>::iterator it = data_count_vector->begin();
        it != data_count_vector->end(); it++) {
        _ks_data_count.push_back((*it)->AsCount());
    }
    _ks_max_counter = max_counter;
}

void plugin::Training_Bin::Plugin::ChangePrefix(StringVal* prefix)
{
    std::string temp((const char*) prefix->Bytes());
    _prefix.swap(temp);
}

// print data when we flush the connection from the dictionary
void plugin::Training_Bin::Plugin::RemoveConnection(StringVal* UID, Val* direction, Val* tag)
{
	std::string UID_str((const char*) UID->Bytes());
    std::unordered_map<std::string, std::shared_ptr<BiFlow> >::iterator 
			got = _flow_dict.find(UID_str);

    // if the connection exists
    if (got != _flow_dict.end())
	{
        std::shared_ptr<IntervalTraining> flow = got->second->get(direction->AsEnum(), tag->AsEnum());

        // only print KS data for individual flows
        if ((UID_str != "ALL") && (_ks_counter < _ks_max_counter))
        {
            // weed out small training data
            if (flow->get_data_length() >= _ks_threshold)
            {
                // iterate over all the required normal data sizes
                for (std::vector<unsigned int>::iterator it = _ks_data_count.begin();
                    it != _ks_data_count.end(); it++) 
                {
                    std::string name = _prefix + "_KS_" + to_string(_ks_counter) + "_" + to_string(*it);
                    print_data(flow->get_data(), name, *it);
                }
                _ks_counter++;
            }
        }
        // only print Interval data for aggregation of all data points and it is big enough
        if ((UID_str == "ALL") && (flow->get_data_length() >= _bin_threshold))
        {
            // iterate over all the bin numbers
            for (std::vector<unsigned int>::iterator it = _bin_counts.begin();
                it != _bin_counts.end(); it++) 
            {
                std::string name = _prefix + "_Interval_" + to_string(*it);
                print_pairs(flow->get_equiprobable_interval(*it), name);
            }
        }
	    _flow_dict.erase(UID_str);
	}
}

std::shared_ptr<IntervalTraining> plugin::Training_Bin::Plugin::getFlow(std::string UID_str, int direction, int tag)
{
    std::unordered_map<std::string, std::shared_ptr<BiFlow> >::iterator 
		got = _flow_dict.find(UID_str);

	if (got == _flow_dict.end())
	{
		std::shared_ptr<BiFlow> biflow = std::shared_ptr<BiFlow> (new BiFlow() ); 

		pair<std::unordered_map<std::string, std::shared_ptr<BiFlow> >::iterator, bool>
			result = _flow_dict.emplace(UID_str, biflow);

		if (!result.second) 
			printf("error in initializing training per flow\n");

        return biflow->get(direction, tag);
	}
	return got->second->get(direction, tag);
}

void plugin::Training_Bin::Plugin::add_feature(StringVal* UID, Val* direction, double feature, Val* tag_val)
{
    std::string UID_str((const char*) UID->Bytes());
	int tag = tag_val->AsEnum();

	// sanity check for tag
	if (tag < 0) { printf("error: invalid Tag %d\n", tag); return; }

	if ((unsigned int) tag >= _tag_count) {_tag_count = (unsigned int) tag + 1;}

    // add the feature both to the individual flow and aggregate of all flows
    getFlow(UID_str, direction->AsEnum(), tag)->add_feature(feature);
    getFlow("ALL", 0, tag)->add_feature(feature);
}
