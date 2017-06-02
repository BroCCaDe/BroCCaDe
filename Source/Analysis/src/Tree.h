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
