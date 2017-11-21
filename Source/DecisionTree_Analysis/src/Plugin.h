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
* Plugin.h : Bro plugin which hold a number of tree models. This plugin contains*
*       the implementation of the functions specified in the decisiontree.bif   *
* Contents :                                                                    *
*   * Init : initialize the plugin with the number of models                    *
*   * LoadModel : load the model from the file specified by the input string    *
*   * Classify : classify input vector according to the model specified. The    *
*           order of the values in the feature vector must match the order of   *
*           features in the model                                               *
*   * Classify_with_strings :                                                   *
*           same as classify with additional input of string                    *
*           vector indicating the name of the feature. The feature vector does  *
*           not have to be in order as the plugin will match the feature names  *
*           specified in the model and the string vector.                       *
*   * Classify_record :                                                         * 
*           similar to classify except that the input is a record.              *
*           Note that the feature names specified in the model must exists in   *
*           the input record otherwise the execution will terminate             *
*   * class_result_event :                                                      *
*           event raised by the plugin specifying the class of the input vector.*
\*******************************************************************************/


#ifndef BRO_PLUGIN_ANALYSIS_DECISIONTREE
#define BRO_PLUGIN_ANALYSIS_DECISIONTREE

#include <plugin/Plugin.h>
#include <unordered_map>    // unordered_map
#include <memory>           // unique_ptr
#include "classifier_c45.h" // c45 classifiers

namespace plugin {
namespace Analysis_DecisionTree {

class Plugin : public ::plugin::Plugin
{
public:
	// load a classifier identified by the file name
	// on index specified by ID_val
	int LoadModel(Val* ID_val, StringVal* model_name);
	
	// classify the features using a classifiers[ID_Val]
	// Classify takes as input the vector of features with the assumption
	// that the features are already in the correct order
	int Classify(Val* ID_val, Val* conn_ID, Val* features, bool send_event=true);
	// Classify_with_strings takes as input a vector of veatures and a vector
	// of strings specifying the name of the features. The name of the feature
	// is matched against the features in the decision tree, so the order of
	// the features can be arbitrary
    int Classify_with_strings(Val* ID_val, Val* conn_ID, Val* features, Val* features_str, bool send_event=true);
	// Classify_record behaves like Classify_with_strings, with the difference that
	// the input is in the form of a record where the feature names are the record's fields.
	// Due to the behaviour of Bro's record lookup, if a wrong record is passed
	// (i.e. a record missing a feature), then the program stops.
	int Classify_record(Val* ID_val, Val* conn_ID, Val* features, bool send_event=true);
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
private:
	// vector of classifiers
	std::unordered_map<int, std::unique_ptr<c45_classifier>> _classifiers;
};

extern Plugin plugin;

}
}

#endif
