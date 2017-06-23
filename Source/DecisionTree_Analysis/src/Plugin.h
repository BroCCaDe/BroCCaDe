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


#ifndef BRO_PLUGIN_ANALYSIS_DECISIONTREE
#define BRO_PLUGIN_ANALYSIS_DECISIONTREE

#include <plugin/Plugin.h>
#include <vector>		// vector
#include <memory>		// unique_ptr
#include "classifier_c45.h"	// c45 classifiers

namespace plugin {
namespace Analysis_DecisionTree {

class Plugin : public ::plugin::Plugin
{
public:
	// Re-initialize the container for the classifiers
	void Init(unsigned int n);
	// load a classifier identified by the file name
	// on index specified by ID_val
	void LoadModel(Val* ID_val, StringVal* model_name);
	
	// classify the features using a classifiers[ID_Val]
	// Classify takes as input the vector of features with the assumption
	// that the features are already in the correct order
	void Classify(Val* ID_val, Val* features);
	// Classify_with_strings takes as input a vector of veatures and a vector
	// of strings specifying the name of the features. The name of the feature
	// is matched against the features in the decision tree, so the order of
	// the features can be arbitrary
	void Classify_with_strings(Val* ID_val, Val* features, Val* features_str);
	// Classify_record behaves like Classify_with_strings, with the difference that
	// the input is in the form of a record where the feature names are the record's fields.
	// Due to the behaviour of Bro's record lookup, if a wrong record is passed
	// (i.e. a record missing a feature), then the program stops.
	void Classify_record(Val* ID_val, Val* features);
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
private:
	// vector of classifiers
	std::vector<std::unique_ptr<c45_classifier>> _classifiers;
};

extern Plugin plugin;

}
}

#endif
