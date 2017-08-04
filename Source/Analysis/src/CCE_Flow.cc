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
* CCE_Flow.cc : Implements CCE_Flow.h                                           *
\*******************************************************************************/

#include "CCE_Flow.h"
#include "Bin_Strategy.h"           // bin allocation superclass
#include "Pattern_Data_Container.h" // tree data structure
#include <math.h>                   // log
#include <memory>                   // unique_ptr and shared_ptr

#ifdef DEBUG_H
#include <stdio.h>                  // printf
#endif

using namespace CCD;

// do a Breadth First Traversal from the root of the tree, this way we know that 
// the next child should be on the same level or one level below (simpler to check)
double CCE::calculate_metric()
{
//	std::shared_ptr<CountingTree> root;		// the root of the tree
    std::shared_ptr< std::vector< std::vector< TreeNode > > > tree;
	unsigned long total_pattern;			// used to count perc and prob
	unsigned long count;				
	unsigned short current_level;
	unsigned short unique_counter = 0;
	double H_prev = 0.0;				// H(X1,...,Xm-1)
	double H_current = 0.0;				// H(X1,...,Xm)
	double H_1 = 0.0;				    // H(X1) = EN(X1)
	double prob;					    // P(x)
	double min_CCE;					    // minimum CCE over various pattern length

	// get the root of the tree
	tree = (static_cast<Pattern_Data*>(_data.get()))->get_tree();		
	total_pattern = (*tree)[0][0].count;

    // calculate EN(X1)
    for (std::vector<TreeNode>::iterator it = (*tree)[1].begin(); it != (*tree)[1].end(); it++)
    {
        count = it->count;
		// get the probability
		prob = (double) count / (double) total_pattern;
		// calculate the entropy
		H_1 = H_1 - prob * log(prob);
    }

    min_CCE = H_1;
    for (size_t i = 2; i < (*tree).size(); i++)
    {
        for (std::vector<TreeNode>::iterator it = (*tree)[i].begin(); it != (*tree)[i].end(); it++)
        {
            // get the probability
		    unsigned long count = it->count;
		    prob = (double) count / (double) total_pattern;

		    // calculate the entropy of the current level
		    H_current = H_current - prob * log(prob);

		    // check if it is unique, if it is add unique counter
		    if (count == 1) unique_counter++;
        }
        double perc = (double) unique_counter / (double) total_pattern; 
		// calculate current CCE (pattern of length m)
		double current_CCE = (H_current - H_prev) + perc * H_1;
#ifdef DEBUG_H
		printf("H%d: %lf\n", current_level, (H_current - H_prev) + perc * H_1);
#endif
		// remember the minimum CCE value
		if (current_CCE < min_CCE) min_CCE = current_CCE;

		// reset the accummulation
		H_prev = H_current;
		H_current = 0.0;
		unique_counter = 0;
    }

	return min_CCE;
}
