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
* Pattern_Data_Container.h : Data container for CCE. Its main purpose is to     *
*       wrap the pattern tree data structure to interface with the general      *
*       analysis tool and holds the bin allocator                               *
\*******************************************************************************/

#ifndef AUX_PLUGINS_PATTERN_DATA_CONTAINER_H
#define AUX_PLUGINS_PATTERN_DATA_CONTAINER_H

#include <list>			// list
#include <memory>		// shared_ptr
#include "Tree.h"		// pattern tree
#include "Bin_Strategy.h"	// abstract Bin allocator
#include "Data_Container.h"	// abstract Data_Container

namespace CCD {

struct TreeNode
{
    unsigned long count;
    unsigned short children[10];
};

class Pattern_Data : public Data_Container
{
public:
	Pattern_Data(unsigned short pattern_length, 
		std::shared_ptr<Bin_Strategy> binner);
	virtual ~Pattern_Data() {_pattern.clear();}

	// return a boolean indicating whether the calculation is triggered
	virtual void add_feature(double feature);

//	std::shared_ptr<CountingTree> get_root() {return _root;}
    std::shared_ptr< std::vector<std::vector<TreeNode> > > get_tree() {return _tree;}
private:
	unsigned short _pattern_length;		

	// The internal of the data container is represented as a tree of depth equal to the
	// pattern length. On each node, possible children are possible bin values.
//	std::shared_ptr<CountingTree> _root;	// the root of the tree data structure
	std::list<unsigned short> _pattern;	// moving window of pattern
	std::shared_ptr<Bin_Strategy> _binner;	// bin allocator

    void add_pattern(std::list<unsigned short>::iterator current, std::list<unsigned short>::iterator end,
        unsigned short id, unsigned short level);
    std::shared_ptr< std::vector<std::vector<TreeNode> > > _tree;
    
};

}

#endif
