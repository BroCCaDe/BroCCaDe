#ifndef AUX_PLUGINS_AUTOCORRELATION_FLOW_H
#define AUX_PLUGINS_AUTOCORRELATION_FLOW_H

#include <vector>		// contains vector
#include <memory>
#include "Analysis.h"		// the superclass (flow analyzer)

namespace CCD {

#define DEFAULT_NUM_LAG 1

// Compute autocorrelation for various lags
class Autocorrelation : public FeatureAnalyzer {
public:
	// Constructor: specify the window size and step size for the data series
	// If there are no lags specified, it contains 
	// the sole DEFAULT_NUM_LAG
	Autocorrelation(std::shared_ptr<Raw_Data> data, 
		std::shared_ptr<std::vector<unsigned int>> lag) :
		FeatureAnalyzer(data), _lag(lag) {}
	virtual ~Autocorrelation() {};

	// Calculate the metric by constructing the class to compute autocorrelation,
	// calculate the mean, the variance, and autocorrelation for various lags.
	virtual double calculate_metric();
private:
	std::shared_ptr<std::vector<unsigned int>> _lag;	// array of lag values
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
	std::vector<double> _data;	// data series
	double _mean;			// calculated mean
	double _var;			// calculated variance
};

}

#endif
