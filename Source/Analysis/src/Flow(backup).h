#ifndef AUX_PLUGINS_FLOW_H
#define AUX_PLUGINS_FLOW_H

#include <vector>

#define EPS1 0.001
#define EPS2 1.0e-8

class Flow_IAT {
public:
	Flow_IAT(unsigned int window_size, unsigned int step);
	~Flow_IAT() {if (_data) free(_data);}
	
	// return the calculated metric if the analysis is triggered
	// otherwise just return -1
	double add_IAT(double timestamp);

	void set_window_size(int window_size) {_window_size = window_size;}
	unsigned int get_window_size() {return _window_size;}
	void set_step(int step) {_step = step;}
	unsigned int get_step() {return _step;}
protected:
	virtual double CalculateMetric() = 0;

	std::vector<double>* _data;
	unsigned int _data_len;
	unsigned int _window_size; // the window size of the number of data
	unsigned int _step; // indicate the interval of window size, i.e. the
			// number of data to empty each calculation triggered
private:
	void ResetWindow();
};

class KS_Flow_IAT : public Flow_IAT {
public:
	KS_Flow_IAT(unsigned int window_size, unsigned int step);
	~KS_Flow_IAT() {if (_normal_data) free(_normal_data);};

	void SetNormalData(std::vector<double> normal_data, int n);
protected:
	virtual double CalculateMetric();
private:
	std::vector<double>* _normal_data;
	int _normal_data_len;

	// this function does the kolmogorov-smirnov test between two data sets
	// (taken directly from the numerical recipees
	void kstwo(std::vector<double> data1, unsigned int n1, 
		std::vector<double> data2, unsigned int n2, double *d, double *prob);
	double probks(double alam);
};

#endif
