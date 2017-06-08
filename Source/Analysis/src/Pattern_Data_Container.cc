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
