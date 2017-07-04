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

	_flow_dict = new std::unordered_map<std::string, 
		std::vector<IntervalTraining*>* >();
	_tag_count = DEFAULT_TAG_COUNT;
	_bin_count = DEFAULT_BIN_COUNT;

	return config;
	}

void plugin::Training_Bin::Plugin::RemoveConnection(StringVal* UID)
{
	std::string UID_str((const char*) UID->Bytes());
	_flow_dict->erase(UID_str);
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

void print_data(std::vector<double> data, std::string file_name)
{
	if (data.size() <= 0) return;
	std::vector<double>::const_iterator it;
	ofstream f;
  	f.open (file_name);
	if (!f.is_open()) {printf("(data) can't open file\n"); return;}
	f << data.size() << "\n";
	for (it = data.begin(); it != data.end(); it++) f << *it << "\n"; 
	f.close();
}

void plugin::Training_Bin::Plugin::add_feature(StringVal* UID, 
	double feature, Val* tag_val)
{
	std::string *UID_str = new std::string((const char*) UID->Bytes());	
	int tag = tag_val->AsEnum();

	// sanity check for tag
	if (tag < 0)
	{
		printf("error: invalid Tag %d\n", tag);
		return;
	}

	if ((unsigned int) tag >= _tag_count) {_tag_count = (unsigned int) tag + 1;}

	std::unordered_map<std::string, std::vector< IntervalTraining*>* >::iterator 
			got = _flow_dict->find(*UID_str);

	std::vector<IntervalTraining*>* flow;
	if (got == _flow_dict->end())
	{
		flow = new std::vector<IntervalTraining*> (_tag_count, NULL);

		pair<std::unordered_map<std::string, 
			std::vector<IntervalTraining*>* >::iterator, bool>
			result = _flow_dict->emplace(*UID_str, flow);

		if (!result.second) 
			printf("error in initializing training per flow\n");
	}
	else
	{ 
		flow = got->second;
		// dynamically adjust the number of analysis set
		if (flow->size() < _tag_count)
			flow->resize(_tag_count);
	}

	if (ACCESS(flow, tag) == NULL)
	{
		ACCESS(flow, tag) = new IntervalTraining();
	}

	ACCESS(flow, tag)->add_feature(feature);
	if (ACCESS(flow, tag)->get_data_length() >= TRAINING_DATA_NUMBER)
	{
		std::string name = "KS_data_" + (*UID_str) + "_" + to_string(tag);
		print_data(ACCESS(flow, tag)->get_data(), name);
		name = "Interval_data_" + (*UID_str) + "_" + to_string(tag);
		print_pairs(ACCESS(flow, tag)->get_equiprobable_interval(_bin_count), name);
		IntervalTraining* temp = ACCESS(flow, tag);
		ACCESS(flow, tag) = NULL;
		delete temp;
	}
}
