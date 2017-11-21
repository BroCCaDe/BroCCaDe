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
* Tree.cc : Implements Tree.h                                                   *
\*******************************************************************************/

#include "Tree.h"
#include <malloc.h>
#include <vector>   // vector
#include <memory>   // unique_ptr

#ifdef DEBUG_H
#include <stdio.h>  // printf
#endif

using namespace CCD;

// Constructor: assign a level to the node and specify the number of children
// it should have
CountingTree::CountingTree(unsigned short bin_count, unsigned short level)
{
	_count = 0; 
	_level = level; 
	_bin_count = bin_count; 
	_children.resize(bin_count, std::shared_ptr<CountingTree>() );
}

// add a pattern recursively, stops when it reaches the "end"
void CountingTree::add_pattern(std::list<unsigned short>::iterator current,
	std::list<unsigned short>::iterator end)
{
	_count++;
	if (current == end) return;		// reaches the end
	unsigned short children_idx = *current;
#ifdef DEBUG_H
	printf("adding %d to level %d (%d)\n", children_idx, _level, _bin_count );
#endif
	// if the children is not yet instantiated, create a new node
	if (_children[children_idx].get() == NULL) 
		_children[children_idx] = std::shared_ptr<CountingTree>
			(new CountingTree(_bin_count, _level+1));

	// add the subsequent symbols in the pattern
	_children[children_idx]->add_pattern(++current, end);
}

// add any non-null child node to the list of nodes
std::list<std::shared_ptr<CountingTree>> CountingTree::get_children()
{
	unsigned short i;
	std::list<std::shared_ptr<CountingTree> > children;
	for (i = 0; i < _bin_count; i++)
	{
		if (_children[i].get() != NULL) children.push_back(_children[i]);
	}
	return children;
}
