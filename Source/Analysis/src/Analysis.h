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
* Analysis.h : super class for all of the analysis engine and a simple analyzer *
* content :                                                                     *
*   * DATA_CONTAINER_TYPE_COUNT : number of data container type, useful for the *
*           mapping between analysis engine and data container                  *
*   * Data_Container_Enum : enumeration on the type of data containers          *
*   * FeatureAnalyzer : abstract class for the analysis engine which contains a *
*           pointer to the data container and method to calculate metric        *
*   * NullAnalysis : the analysis engine which just returns the stored data     *
\*******************************************************************************/

#ifndef AUX_PLUGIN_ANALYSIS_H
#define AUX_PLUGIN_ANALYSIS_H

#include "Data_Container.h"	                // Data_Container
#include <memory>		                    // shared_ptr

namespace CCD {

// this has to match how many data countainer types there are
const int DATA_CONTAINER_TYPE_COUNT = 5;

enum Data_Container_Enum
{
	RAW_DATA = 0,
	HISTOGRAM_DATA = 1,
	PATTERN_DATA = 2,
	REGULARITY_DATA = 3,
	NULL_DATA = 4
};

class FeatureAnalyzer
{
public:
	FeatureAnalyzer(std::shared_ptr<Data_Container> data) : _data(data) {}
	virtual ~FeatureAnalyzer() {}

	virtual double calculate_metric() = 0;  // abstract method to calculate metric
protected:
	std::shared_ptr<Data_Container> _data;  // pointer to the data container
};

class NullAnalysis : public FeatureAnalyzer
{
public:
	NullAnalysis(std::shared_ptr<Null_Data> data) : FeatureAnalyzer(data) {}
	virtual ~NullAnalysis() {}

	virtual double calculate_metric()
	{
        // return the contained data
		return (static_cast<Null_Data*>(_data.get()))->get_data();
	}
};


}

#endif
