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
* Autocorrelation_Flow.cc : implements Autocorrelation_Flow.h			        *
\*******************************************************************************/

#include <vector>
#include <cmath>
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
    _mean = _mean + (double) (_data[i] / (double) N);
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
    _var = _var + (double) (pow((_data[i] - _mean), 2.0) / (double) N);
}

//===========================================================================
//=  Function to compute autocorrelation for a series X                     =
//=   - Corrected divide by N to divide (N - lag) from Tobias Mueller       =
//===========================================================================
double Autocorrelation_Calculation::compute_autoc(unsigned int lag)
{
  // sanity check : variance should never be zero
  if (_var == 0.0) {return 0.0;}

  double   autocv;      // Autocovariance value
  double   ac_value;    // Computed autocorrelation value to be returned
  unsigned int i;       // Loop counter
  int      N;           // Size of data

  // Loop to compute autovariance
  autocv = 0.0;
  N = _data.size();
  for (i=0; i<(N - lag); i++)
    autocv = autocv + ((_data[i] - _mean) * (_data[i+lag] - _mean));
  autocv = (1.0 / (double) (N - lag)) * autocv;

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
    if (_lag == 0) _lag = DEFAULT_NUM_LAG;

	// sum the autocorrelation for all possible lag values
	for (i = 0; i <= _lag && i < contained_data.size(); i++)
	{
#ifdef DEBUG
        printf("%d ", i);
#endif
        double ac_value = ac_compute.compute_autoc(i);
		sum += std::abs(ac_value); // autocorrelation for a particular lag
	}
#ifdef DEBUG
        printf("\n");
#endif
	return sum / (double) (_lag + 1.0); // 1/N * sum(autocorrelation for all lags)
}
