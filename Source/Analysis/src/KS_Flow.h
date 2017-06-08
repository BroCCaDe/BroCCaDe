#ifndef AUX_PLUGINS_KS_FLOW_H
#define AUX_PLUGINS_KS_FLOW_H

#include <vector>	// vector
#include "Data_Container.h"
#include "Analysis.h"	// feature analyzer superclass

// define the epsilon (smallest significant fraction)
#ifndef AUX_PLUGINS_EPS_CONST
#define EPS1 0.001
#define EPS2 1.0e-8
#endif

namespace CCD {
/*
// flow analyzer which uses kolmogorov-smirnov test
class KS : public FlowAnalyzer {
public:
	// Initialize with window_size and step_size to create the data window,
	// also the "normal" data series
	KS(unsigned int window_size, unsigned int step_size,
		std::shared_ptr<std::vector<double>> normal_data);
	virtual ~KS() {}

	// Set the step size of the data container
	virtual void set_step_size(unsigned int step_size);
protected:
	virtual double calculate_metric();
private:
	std::shared_ptr<std::vector<double>> _normal_data;	// normal data series

	// these functions does the kolmogorov-smirnov test between two data sets
	// (taken directly from the numerical recipes)
	void kstwo(std::vector<double> data1, unsigned int n1, 
		std::vector<double> data2, unsigned int n2, double *d, double *prob);
	double probks(double alam);
};*/

// flow analyzer which uses kolmogorov-smirnov test
class KS : public FeatureAnalyzer {
public:
	// Initialize with window_size and step_size to create the data window,
	// also the "normal" data series
	KS(std::shared_ptr<Raw_Data> data, std::shared_ptr<std::vector<double>> normal_data) :
		FeatureAnalyzer(data), _normal_data(normal_data) {}
	virtual ~KS() {}

	virtual double calculate_metric();
private:
	std::shared_ptr<std::vector<double>> _normal_data;	// normal data series

	// these functions does the kolmogorov-smirnov test between two data sets
	// (taken directly from the numerical recipes)
	void kstwo(std::vector<double> data1, unsigned int n1, 
		std::vector<double> data2, unsigned int n2, double *d, double *prob);
	double probks(double alam);
};

}

#endif
