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

namespace plugin { namespace Feature_Training { Plugin plugin; } }

using namespace plugin::Feature_Training;

plugin::Configuration Plugin::Configure()
	{
	plugin::Configuration config;
	config.name = "Feature::Training";
	config.description = "<Insert description>";
	config.version.major = 0;
	config.version.minor = 1;

	return config;
	}

std::shared_ptr<FeatureTraining> plugin::Feature_Training::Plugin::find_or_create(int ID)
{
    if (_data.size() <= ID) _data.resize(ID+1);
    if (_data[ID].get() == NULL)
        _data[ID] = std::shared_ptr<FeatureTraining> (new FeatureTraining());
    
	return _data[ID];
}

void plugin::Feature_Training::Plugin::ChangeRelation(Val* ID, StringVal* name)
{
    int set_ID = ID->AsEnum();
    std::string *name_str = new std::string((const char*) name->Bytes());
    std::shared_ptr<FeatureTraining> d = find_or_create(set_ID);
    d->change_relation(*name_str);
}

void plugin::Feature_Training::Plugin::Add_Attributes(Val* ID, StringVal* name)
{
    int set_ID = ID->AsEnum();
	std::string *name_str = new std::string((const char*) name->Bytes());
	std::shared_ptr<FeatureTraining> d = find_or_create(set_ID);
	d->add_attribute(*name_str);
}

void plugin::Feature_Training::Plugin::Add_Class(Val* ID, StringVal* name)
{
    int set_ID = ID->AsEnum();
	std::string *name_str = new std::string((const char*) name->Bytes());
	std::shared_ptr<FeatureTraining> d = find_or_create(set_ID);
	d->add_class(*name_str);
}

void plugin::Feature_Training::Plugin::Add_DataRow(Val* ID, Val* values, StringVal* name)
{
    int set_ID = ID->AsEnum();
	std::string *name_str = new std::string((const char*) name->Bytes());
	std::shared_ptr<FeatureTraining> d = find_or_create(set_ID);
	std::vector<Val*>* values_vector = values->AsVector();
	std::vector<double> features;

	for (unsigned int i = 0; i < values_vector->size(); i++)
		features.push_back((*values_vector)[i]->AsDouble());

	d->add_data_row(features, *name_str);
}

void plugin::Feature_Training::Plugin::print(Val* ID, StringVal* FileName)
{
    int set_ID = ID->AsEnum();
	std::string *file_str = new std::string((const char*) FileName->Bytes());
	ofstream of;
	std::shared_ptr<FeatureTraining> d = find_or_create(set_ID);
	of.open (*file_str);
  	of << d->get_relation() << d->get_attributes() << d->get_classes() << d->get_data();
  	of.close();
}
