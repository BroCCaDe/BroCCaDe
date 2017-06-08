#include "CCE_Flow.h"
#include "Bin_Strategy.h"		// bin allocation superclass
#include "Pattern_Data_Container.h"	// tree data structure
#include <math.h>			// log
#include <stdio.h>			// printf
#include <memory>			// unique_ptr and shared_ptr

using namespace CCD;

/*
CCE::CCE(unsigned short step_size, unsigned short pattern_size, 
		std::shared_ptr<Bin_Strategy> binner)
	: FlowAnalyzer(std::unique_ptr<Pattern_Data_Container> 
		(new Pattern_Data_Container(pattern_size, binner)), step_size)
{}
*/

// do a Breadth First Traversal from the root of the tree, this way we know that 
// the next child should be on the same level or one level below (simpler to check)
double CCE::calculate_metric()
{
	std::shared_ptr<CountingTree> root;		// the root of the tree
	unsigned long total_pattern;			// used to count perc and prob
	unsigned long count;				
	unsigned short current_level;
	unsigned short unique_counter = 0;
	double H_prev = 0.0;				// H(X1,...,Xm-1)
	double H_current = 0.0;				// H(X1,...,Xm)
	double H_1 = 0.0;				// H(X1) = EN(X1)
	double prob;					// P(x)
	double min_CCE;					// minimum CCE over various pattern length
	std::list<std::shared_ptr<CountingTree>> queue, children;

	// get the root of the tree
	root = (static_cast<Pattern_Data*>(_data.get()))->get_root();	
	total_pattern = root->get_count();

	// count the first entropy EN(X1) by iterating over nodes of level 1
	children = root->get_children();
	queue.insert(queue.end(), children.begin(), children.end());
	
	std::list<std::shared_ptr<CountingTree>>::iterator it = queue.begin();
#ifdef DEBUG
	printf("total pattern : %lu\n", total_pattern);
#endif
	// calculate EN(X1), also put the children (level 2) on the queue
	while (it != queue.end() && (*it)->get_level() == 1)
	{
		count = (*it)->get_count();
		// get the probability
		prob = (double) count / (double) total_pattern;
		// calculate the entropy
		H_1 = H_1 - prob * log(prob);

		// add the children (level 2) and pop the current node from the queue
		children = (*it)->get_children();
		queue.insert(queue.end(), children.begin(), children.end());
		it++;
		queue.pop_front();
	}
#ifdef DEBUG
	printf("H1 : %lf\n", H_1);
#endif
	H_prev = H_1;
	min_CCE = H_1;

	// scan all the children
	current_level = 2;
	while (queue.size() > 0)
	{
		// get the level of the queue header
		std::shared_ptr<CountingTree> current = queue.front();
		unsigned short node_level = queue.front()->get_level();

		// if it is different level, then calculate the outstanding entropy
		if (node_level > current_level)
		{
			// calculate perc(Xm)
			double perc = (double) unique_counter / (double) total_pattern; 
			// calculate current CCE (pattern of length m)
			double current_CCE = (H_current - H_prev) + perc * H_1;
#ifdef DEBUG
			printf("H%d: %lf\n", current_level, (H_current - H_prev) + perc * H_1);
#endif
			// remember the minimum CCE value
			if (current_CCE < min_CCE) min_CCE = current_CCE;

			// reset the accummulation
			H_prev = H_current;
			H_current = 0.0;
			current_level = node_level;
			unique_counter = 0;
		}
		// insert children from the current tree
		children = current->get_children();
		queue.insert(queue.end(), children.begin(), children.end());

		// get the probability
		unsigned long count = current->get_count();
		prob = (double) count / (double) total_pattern;

		// calculate the entropy of the current level
		H_current = H_current - prob * log(prob);

		// check if it is unique, if it is add unique counter
		if (count == 1) unique_counter++;

		// pop from the queue
		queue.pop_front();
	}

	// final check : if there is oustanding entropy
	if (H_current > 0.0)
	{
		double perc = (double) unique_counter / (double) total_pattern;
		double current_CCE = (H_current - H_prev) + perc * H_1;
#ifdef DEBUG
		printf("H%d: %lf\n", current_level, current_CCE);
#endif
		if (current_CCE < min_CCE) min_CCE = current_CCE;
	}

	return min_CCE;
}
