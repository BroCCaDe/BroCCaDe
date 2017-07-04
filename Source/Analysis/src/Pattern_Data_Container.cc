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
* Pattern_Data_Container.cc : Implements Pattern_Data_Container.h               *
\*******************************************************************************/

#include "Pattern_Data_Container.h"
#include "Bin_Strategy.h"           // bin allocator
#include <list>                     // list
#include <memory>                   // shared_ptr

//#define DEBUG_H

#ifdef DEBUG_H
#include <stdio.h>
#endif

using namespace CCD;



// Initialize the data container with the bin allocator and creating new tree root
Pattern_Data::Pattern_Data(unsigned short pattern_length, 
		std::shared_ptr<Bin_Strategy> binner)
	: Data_Container()
{
	_binner = binner;
//	std::shared_ptr<CountingTree> tmp_root(new CountingTree(binner->get_bin_count(), 0));
//	_root.swap(tmp_root);
    std::shared_ptr<std::vector<std::vector<TreeNode> > > 
        tmp_tree(new std::vector< std::vector< TreeNode> >(pattern_length+1));
    _tree.swap(tmp_tree);
    (*_tree)[0].push_back({0, {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}});
	_pattern_length = pattern_length;
}

void Pattern_Data::add_feature(double feature)
{
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
	for (; it != _pattern.end(); it++) printf("%d ", *it);
	printf("\n");
#endif	

	// only add the pattern to the tree if it is already full
	if (_pattern.size() >= _pattern_length)
	{
		// add pattern to the tree
		//_root->add_pattern(_pattern.begin(), _pattern.end());
        add_pattern(_pattern.begin(), _pattern.end(), 0, 0);
		// and remove the last data point in the pattern
		_pattern.pop_back();
	}
}

// append
void Pattern_Data::add_pattern(std::list<unsigned short>::iterator current, 
    std::list<unsigned short>::iterator end, short id, unsigned short level)
{
    (*_tree)[level][id].count++;
    if (current == end) return;
    short next_id = (*_tree)[level][id].children[*current];
    if(next_id == -1)
    {
        TreeNode newchild = {0, {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}};
    //    newchild.count = 0;
    //    memset(newchild.children, 10 * sizeof (unsigned short), 0);
        (*_tree)[level+1].push_back(newchild);
        next_id = (*_tree)[level][id].children[*current] = (*_tree)[level+1].size()-1;
    }
    add_pattern(++current, end, next_id, level+1);
}
