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
\*******************************************************************************/

#include "Pattern_Data_Container.h"
#include "Bin_Strategy.h"
#include <list>
#include "stdio.h"
#include <memory>

//#define DEBUG_H

using namespace CCD;

Pattern_Data::Pattern_Data(unsigned short pattern_length, 
		std::shared_ptr<Bin_Strategy> binner)
	: Data_Container()
{
//	printf("Pattern_Data::Pattern_Data got here\n");
	_binner = binner;
	std::shared_ptr<CountingTree> tmp_root(new CountingTree(binner->get_bin_count(), 0));
	_root.swap(tmp_root);
	_pattern_length = pattern_length;
}

void Pattern_Data::add_feature(double feature)
{
//	printf("Pattern_Data::add_feature got here\n");
	unsigned short bin_number = _binner->get_bin_number(feature);
	unsigned short bin_count = _binner->get_bin_count();
	// sanity check: if the bin number is out of bounds
	if (bin_number < 0 || bin_number > bin_count) 
	{
		printf("Weird bin number : %d (0 - %d)\n", bin_number, bin_count);
		return;
	}

	_pattern.push_front(bin_number);

#ifdef DEBUG_H
	// print the pattern
	std::list<unsigned short>::const_iterator it = _pattern.begin();
	printf("pattern (%lu): ", _root->get_count());
	for (; it != _pattern.end(); it++)
	{
		printf("%d ", *it);
	}
	printf("\n");
#endif	

	// only add the pattern to the tree if it is already full
	if (_pattern.size() >= _pattern_length)
	{
		// add pattern to the tree
		_root->add_pattern(_pattern.begin(), _pattern.end());
		// and remove the last data point in the pattern
		_pattern.pop_back();
	}
}
