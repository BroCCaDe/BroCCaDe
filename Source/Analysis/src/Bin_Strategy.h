/*******************************************************************************\
* Copyright (c) 2017 by Hendra Gunadi (Hendra.Gunadi@murodch.edu.au)            *
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
* Bin_Strategy.h : superclass for the bin allocator and a simple bin allocator  *
* Content :                                                                     *
*   * class Bin_Strategy : the superclass definition which contains 2 abstract  *
*           methods that all bin allocator have to override, get the total      *
*           number of bins and classifying a feature into a bin                 *
*   * class Bin_Strategy_Null : a bin allocator that directly returns the       *
*           feature as the bin number. Only works for value in unsigned short   *
*           range [0, 2^16)                                                     *
\*******************************************************************************/

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
	// Constructor: specify the possible bin values
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
