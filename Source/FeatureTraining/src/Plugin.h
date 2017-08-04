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
* Plugin.h : Bro plugin which is used to produce training data for Weka. This   *
*       plugin implements the functions in specified in training.bif. Each      *
*       training data is distinguished by the calculation set (int) from the    *
*       analysis plugin.                                                        *
* Contents :                                                                    *
*   * ChangeRelation        : change the relation string                        *
*   * AddAttributes         : add an attribute with a particular name. This will*
*                             corresponds to the feature name for Weka.         *
*   * AddClass              : add an attribute with the class names. This will  *
*                             corresponds to the class names in the classifier. *
*   * AddDataRow            : add comma delimited data and its class            *
*   * print_training_data   : print the training data into a file               *
\*******************************************************************************/

#ifndef BRO_PLUGIN_FEATURE_TRAINING
#define BRO_PLUGIN_FEATURE_TRAINING

#include <plugin/Plugin.h>
#include <memory>
#include "TrainingData.h"

namespace plugin {
namespace Feature_Training {

class Plugin : public ::plugin::Plugin
{
public:
	void ChangeRelation(Val* ID, StringVal* name);
	void Add_Attributes(Val* ID, StringVal* name);
	void Add_Class(Val* ID, StringVal* name);
	void Add_DataRow(Val* ID, Val* values, StringVal* name);
	void print(Val* ID, StringVal* FileName);
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
private:
	std::shared_ptr<FeatureTraining> find_or_create(int ID);

	std::vector<std::shared_ptr<FeatureTraining> > _data;
};

extern Plugin plugin;

}
}

#endif
