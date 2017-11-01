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
* KS_Flow.h : analysis engine to measure the Kolmogorov-Smirnov test on a plain *
*       data against a ``normal'' data obtained from training (in the config)   *
*                                                                               *
* - changed to compare against a set of normal values                           *
* - remove the calculation of the probks because we are not using it            *
\*******************************************************************************/

#ifndef AUX_PLUGINS_KS_FLOW_H
#define AUX_PLUGINS_KS_FLOW_H

#include <vector>		                                // vector
#include <memory>
#include "Data_Container.h"	                            // Raw_Data
#include "Analysis.h"		                            // feature analyzer superclass

// define the epsilon (smallest significant fraction)
#ifndef AUX_PLUGINS_EPS_CONST
#define EPS1 0.001
#define EPS2 1.0e-8
#endif

namespace CCD {

// flow analyzer which uses kolmogorov-smirnov test
class KS : public FeatureAnalyzer {
public:
	// Initialize with the "normal" data series
	KS(std::shared_ptr<Raw_Data> data, std::shared_ptr<std::vector<std::vector<double>>> normal_data) :
		FeatureAnalyzer(data), _normal_data(normal_data) {}
	virtual ~KS() {}

	virtual double calculate_metric();
private:
	std::shared_ptr<std::vector<std::vector<double>>> _normal_data;    // local reference to normal data series

	// these functions does the kolmogorov-smirnov test between two data sets
	// (taken directly from the numerical recipes)
	//void kstwo(std::vector<double> data1, std::vector<double> data2, double *d, double *prob);
    double kstwo(std::vector<double> data1, std::vector<double> data2);
	//double probks(double alam);
};

}

#endif
