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

#ifndef AUX_PLUGINS_COUNTINGTREE_H
#define AUX_PLUGINS_COUNTINGTREE_H

#include "Bin_Strategy.h"	// Bin Allocator
#include <list>			// list
#include <vector>		// vector
#include <memory>		// unique_ptr

namespace CCD {

/*
// Tree data structure to count the patterns. Each node represents
// a symbol in a pattern, e.g. if we have 2 patterns [5,3,1] and [5, 3, 3]
// then the tree is of the form
//    5 (2)    level 1
//    /  
//  3 (2)      level 2
//  /    \
// 1 (1) 3 (1) level 3
*/
class CountingTree
{
public:
	CountingTree(unsigned short bin_count, unsigned short level);
	~CountingTree() {};

	// recursively adds a pattern to the tree, adding count for each
	// symbol appearance
	void add_pattern(std::list<unsigned short>::iterator current,
		std::list<unsigned short>::iterator end);

	// get the list of children of the current node
	std::list<std::shared_ptr<CountingTree> > get_children();

	// get the count of the symbol represented by the node
	unsigned long get_count() {return _count;}
	// get the current level of the node
	unsigned short get_level() {return _level;}
private:
	std::vector<std::shared_ptr<CountingTree> > _children;	// list of children 
	unsigned long _count;					// the count of the symbol
	unsigned short _bin_count;				// possible values
	unsigned short _level;					// current level
};

}

#endif
