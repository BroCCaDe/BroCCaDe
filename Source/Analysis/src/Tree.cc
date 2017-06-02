#include "Tree.h"
#include <malloc.h>
#include <vector>	// vector
#include <memory>	// unique_ptr

#ifdef DEBUG_H
#include <stdio.h>	// printf
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
