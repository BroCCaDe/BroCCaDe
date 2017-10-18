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
* Autocorrelation_Flow.h : Analysis engine that calculates autocorrelation      *
* Content :                                                                     *
*   * class Autocorrelation : an analysis engine which store the data as raw    *
*           data and employs Autocorrelation_Calculation class to calculate its *
*           metric                                                              *
*   * class Autocorrelation_Calculation : a class that calculates the average   *
*           sum of autocorrelation for various lags given a time series         *
\*******************************************************************************/

#ifndef AUX_PLUGINS_AUTOCORRELATION_FLOW_H
#define AUX_PLUGINS_AUTOCORRELATION_FLOW_H

#include <vector>		                                // vector
#include <memory>                                       // shared_ptr
#include "Analysis.h"		                            // the superclass (flow analyzer)

namespace CCD {

#define DEFAULT_NUM_LAG 10

// Compute autocorrelation for various lags
class Autocorrelation : public FeatureAnalyzer {
public:
	// Constructor: specify the window size and step size for the data series
	// If there are no lags specified, it contains 
	// the sole DEFAULT_NUM_LAG
	Autocorrelation(std::shared_ptr<Raw_Data> data, unsigned int lag) :
		FeatureAnalyzer(data), _lag(lag) {}
	virtual ~Autocorrelation() {};

	// Calculate the metric by constructing the class to compute autocorrelation,
	// calculate the mean, the variance, and autocorrelation for various lags.
	virtual double calculate_metric();
private:
	unsigned int _lag;	// array of lag values
};

// This class is adapted from auto.c by Kenneth J. Christensen, University of South Florida.
// The class is used to calculate autocorrelation for various lags.
// The mean and variance of the data needs to be calculated first 
// before we compute the autocorrelation
class Autocorrelation_Calculation {
public:
	// Initialize the class with a copy of the data series
	Autocorrelation_Calculation(std::vector<double> data) {
		_data = data; 
		_mean = 0.0; 
		_var = 0.0;
	}
	virtual ~Autocorrelation_Calculation() {}
	
	// Calculate the autocorrelation of the data series w.r.t. a lag: 
	// autocovariance / variance where autocovariance is
	// sum((data[i] - mean) + (data[i+lag] - mean)) / (N - lag).
	double compute_autoc(unsigned int lag);
	
	// Calculate the mean of the data series : sum(data[i]) / N
	void compute_mean(void);

	// Calculate the variance of the data series : 
	// sum((data[i] - mean)^2) / N
	void compute_variance(void);
private:
	std::vector<double> _data;	                        // data series
	double _mean;			                            // calculated mean
	double _var;			                            // calculated variance
};

}

#endif
