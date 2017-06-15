#include <vector>
#include <math.h>
#include <algorithm>
#include "Autocorrelation_Flow.h"
#include "Data_Container.h"

using namespace CCD;

//===========================================================================
//=  Function to compute mean for a series X                                =
//===========================================================================
void Autocorrelation_Calculation::compute_mean(void)
{
  int      i;           // Loop counter
  int      N;           // Size of data

  // Loop to compute mean
  _mean = 0.0;
  N = _data.size();
  for (i=0; i<N; i++)
    _mean = _mean + (_data[i] / N);
}

//===========================================================================
//=  Function to compute variance for a series X                            =
//===========================================================================
void Autocorrelation_Calculation::compute_variance(void)
{
  int      i;           // Loop counter
  int      N;           // Size of data

  // Loop to compute variance
  _var = 0.0;
  N = _data.size();
  for (i=0; i<N; i++)
    _var = _var + (pow((_data[i] - _mean), 2.0) / N);
}

//===========================================================================
//=  Function to compute autocorrelation for a series X                     =
//=   - Corrected divide by N to divide (N - lag) from Tobias Mueller       =
//===========================================================================
double Autocorrelation_Calculation::compute_autoc(unsigned int lag)
{
  // sanity check : variance should never be zero
  if (_var == 0.0) {
    printf("autocorrelation calculation: variance is zero\n"); return 0.0;
  }

  double   autocv;      // Autocovariance value
  double   ac_value;    // Computed autocorrelation value to be returned
  unsigned int i;       // Loop counter
  int      N;           // Size of data

  // Loop to compute autovariance
  autocv = 0.0;
  N = _data.size();
  for (i=0; i<(N - lag); i++)
    autocv = autocv + ((_data[i] - _mean) * (_data[i+lag] - _mean));
  autocv = (1.0 / (N - lag)) * autocv;

  // Autocorrelation is autocovariance divided by variance
  ac_value = autocv / _var;

  return(ac_value);
}

// Metric calculation
double Autocorrelation::calculate_metric()
{
	unsigned int i;
	double sum = 0.0;
	std::vector<double> contained_data = (static_cast<Raw_Data*>(_data.get()))->get_data();

	// instantiate the class
	Autocorrelation_Calculation ac_compute(contained_data);

	// compute mean and variance
	ac_compute.compute_mean();
	ac_compute.compute_variance();

	// check if there are no lags, add the default value
	// bounded by the size of the data
	if (_lag->size() == 0) _lag->push_back((DEFAULT_NUM_LAG > contained_data.size()) ? 
		contained_data.size() : DEFAULT_NUM_LAG);

	// sum the autocorrelation for all possible lag values
	for (i = 0; i < _lag->size(); i++)
	{
		sum += abs(ac_compute.compute_autoc((*_lag)[i])); // autocorrelation for a particular lag
	}
	return sum / _lag->size(); // 1/N * sum(autocorrelation for all lags)
}
