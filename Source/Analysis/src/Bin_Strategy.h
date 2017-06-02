#ifndef AUX_PLUGINS_BIN_STRATEGY_H
#define AUX_PLUGINS_BIN_STRATEGY_H

namespace CCD {

// Superclass for the bin allocator.
class Bin_Strategy
{
public:
	Bin_Strategy() {};
	virtual ~Bin_Strategy() {};

	// classify a feature into a bin
	virtual unsigned short get_bin_number(double feature) = 0;
	// get the number of possible bin
	virtual unsigned short get_bin_count() = 0;
};

// Simple bin allocator which just take a feature and return it directly
class Bin_Strategy_Null
{
public:
	// Constructor: specify the possible bin
	Bin_Strategy_Null(unsigned short bin_count) : _bin_count(bin_count) {}
	virtual ~Bin_Strategy_Null() {}
	
	// cast the feature into a suitable return value
	virtual unsigned short get_bin_number(double feature) 
		{return static_cast<unsigned short>(feature);}

	virtual unsigned short get_bin_count() {return _bin_count;}
private:
	unsigned short _bin_count;	// maximum possible value
};

}

#endif
