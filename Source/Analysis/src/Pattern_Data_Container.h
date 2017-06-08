#ifndef AUX_PLUGINS_PATTERN_DATA_CONTAINER_H
#define AUX_PLUGINS_PATTERN_DATA_CONTAINER_H

#include <list>			// list
#include <memory>		// shared_ptr
#include "Tree.h"		// pattern tree
#include "Bin_Strategy.h"	// abstract Bin allocator
#include "Data_Container.h"	// abstract Data_Container

namespace CCD {

class Pattern_Data : public Data_Container
{
public:
	Pattern_Data(unsigned short pattern_length, 
		std::shared_ptr<Bin_Strategy> binner);
	virtual ~Pattern_Data() {_pattern.clear();}

	// return a boolean indicating whether the calculation is triggered
	virtual void add_feature(double feature);

	std::shared_ptr<CountingTree> get_root() {return _root;}
private:
	unsigned short _pattern_length;		
	unsigned short _bin_count;

	// The internal of the data container is represented as a tree of depth equal to the
	// pattern length. On each node, possible children are possible bin values.
	std::shared_ptr<CountingTree> _root;	// the root of the tree data structure
	std::list<unsigned short> _pattern;	// moving window of pattern
	std::shared_ptr<Bin_Strategy> _binner;	// bin allocator
};

}

#endif
