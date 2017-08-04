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
* CCE_Flow.h : an analysis engine that calculates CCE. The data itself is stored*
*       as a tree of pattern with a particular length. Based upon the           *
*       description in "An Entropy-Based Approach to Detecting Covert Timing    *
*       Channels" by Steven Gianvecchio and Haining Wang.                       *
\*******************************************************************************/

#ifndef AUX_PLUGINS_CCE_FLOW_H
#define AUX_PLUGINS_CCE_FLOW_H

#include <vector>                   // vector
#include <memory>                   // shared_ptr
#include "Analysis.h"               // superclass (feature analyzer)
#include "Pattern_Data_Container.h" // Pattern_Data

namespace CCD {

class CCE : public FeatureAnalyzer {
public:
	// Constructor: we need to specify the pattern size and the bin allocation strategy

	CCE (std::shared_ptr<Pattern_Data> data) : FeatureAnalyzer(data) {}
	virtual ~CCE () {};

	// We calculate the entropy value by calculating conditional entropy on each level of
	// the tree. In particular, we calculate the minimum of 
	//	CCE (Xm|X1,...,Xm-1) = CE(Xm|X1,...,Xm-1) + perc(Xm) * EN(X1)
	// among patterns of length 1, ..., m
	// where EN(X1) is the entropy for the first level of the tree,
	// perc(Xm) is the percentage of unique patterns of length m,
	// and CE(Xm|X1,...,Xm-1) is the conditional entropy 
	virtual double calculate_metric();
};

}

#endif
