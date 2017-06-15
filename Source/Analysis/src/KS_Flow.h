#ifndef AUX_PLUGINS_KS_FLOW_H
#define AUX_PLUGINS_KS_FLOW_H

#include <vector>		// vector
#include "Data_Container.h"	// Raw_Data
#include "Analysis.h"		// feature analyzer superclass

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
	KS(std::shared_ptr<Raw_Data> data, std::shared_ptr<std::vector<double>> normal_data) :
		FeatureAnalyzer(data), _normal_data(normal_data) {}
	virtual ~KS() {}

	virtual double calculate_metric();
private:
	std::shared_ptr<std::vector<double>> _normal_data;	// local reference to normal data series

	// these functions does the kolmogorov-smirnov test between two data sets
	// (taken directly from the numerical recipes)
	void kstwo(std::vector<double> data1, std::vector<double> data2, double *d, double *prob);
	double probks(double alam);
};

}

#endif
