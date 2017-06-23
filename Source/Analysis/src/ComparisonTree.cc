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

#include "ComparisonTree.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int ComparisonTree::calculate(std::vector<unsigned int> features)
{
	return calculate_rec(features, _tree[0]);
}

unsigned int ComparisonTree::get_result(std::vector<unsigned int> features, 
	CT_Enum::Node_Type type, unsigned int value)
{
	switch(type)
	{
		case CT_Enum::NODE_OPERATION: return calculate_rec(features, _tree[value]);
		case CT_Enum::NODE_FEATURE_INDEX: return features[value];
		case CT_Enum::NODE_VALUE: return value;
		default: break;
	}
	return 0; // shouldn't happen
}

unsigned int ComparisonTree::calculate_rec(std::vector<unsigned int> features, 
	CT_Node* _current_tree)
{
	unsigned int left_result;
	unsigned int right_result;

	left_result = get_result(features, _current_tree->get_left_type(), 
		_current_tree->get_left_value());
	right_result = get_result(features, _current_tree->get_right_type(), 
		_current_tree->get_right_value());

	return CT_Enum::binary_opr(left_result, right_result, 
		_current_tree->get_operation_type());
}


unsigned int CT_Enum::binary_opr(unsigned int lval, 
	unsigned int rval, Operation_Type type)
{
	switch(type)
	{
		case OPERATION_PLUS: return lval + rval;
		case OPERATION_MINUS: return lval - rval;
		case OPERATION_TIMES: return lval * rval;
		case OPERATION_DIVIDE: return lval / rval;
		case OPERATION_AND: return lval && rval;
		case OPERATION_OR: return lval || rval;
		case OPERATION_GT: return lval > rval;
		case OPERATION_GE: return lval >= rval;
		case OPERATION_LT: return lval < rval;
		case OPERATION_LE: return lval <= rval;
		case OPERATION_EQ: 
			{
#ifdef DEBUG
				printf("%u == %u\n", lval, rval); 
#endif
				return lval == rval;
			}
		default: break;
	}
	printf("error : Invalid operation\n");
	// shouldn't happen
	return 0;
}

CT_Enum::Node_Type CT_Enum::switch_type(const char c)
{
	switch (c)
	{
		case 'n' : {return NODE_OPERATION;}
		case 'a' : {return NODE_FEATURE_INDEX;}
		case 'v' : {return NODE_VALUE;}
		default : break;
	}
	printf("Invalid node character : %c\n", c);
	return NODE_INVALID;
}

const char* CT_Enum::node_type_to_str(Node_Type type)
{
	switch(type)
	{
		case NODE_OPERATION : return "NODE_OPERATION";
		case NODE_FEATURE_INDEX : return "NODE_FEATURE_INDEX";
		case NODE_VALUE : return "NODE_VALUE";
		default : break;
	}
	return "NODE_INVALID";
}

const char* CT_Enum::opr_to_str(Operation_Type opr)
{
	switch(opr)
	{
		case OPERATION_PLUS: return "OPERATION_PLUS";
		case OPERATION_MINUS: return "OPERATION_MINUS";
		case OPERATION_TIMES: return "OPERATION_TIMES";
		case OPERATION_DIVIDE: return "OPERATION_DIVIDE";
		case OPERATION_AND: return "OPERATION_AND";
		case OPERATION_OR: return "OPERATION_OR";
		case OPERATION_GT: return "OPERATION_GT";
		case OPERATION_GE: return "OPERATION_GE";
		case OPERATION_LT: return "OPERATION_LT";
		case OPERATION_LE: return "OPERATION_LE";
		case OPERATION_EQ: return "OPERATION_EQ";
		default: break;
	}
	return "OPERATION_INVALID";
}

CT_Enum::Operation_Type CT_Enum::switch_opr(const char c)
{
	switch (c)
	{
		case '+' : {return OPERATION_PLUS;}
		case '-' : {return OPERATION_MINUS;}
		case '*' : {return OPERATION_TIMES;}
		case '/' : {return OPERATION_DIVIDE;}
		case '&' : {return OPERATION_AND;}
		case '|' : {return OPERATION_OR;}
		case '<' : {return OPERATION_LT;}
		case '>' : {return OPERATION_GT;}
		case '=' : {return OPERATION_EQ;}
		case '[' : {return OPERATION_LE;}
		case ']' : {return OPERATION_GE;}
		default : break;
	}
	printf("Invalid operation character : %c\n", c);
	return OPERATION_INVALID;
}

void ComparisonTree::populate_tree(unsigned int index, 
	unsigned int left_value, char left_type_chr, 
	unsigned int right_value, char right_type_chr, 
	char opr_chr)
{
	_tree[index] = new CT_Node
		(left_value, CT_Enum::switch_type(left_type_chr), 
		right_value, CT_Enum::switch_type(right_type_chr), 
		CT_Enum::switch_opr(opr_chr));
}

ComparisonTree* ComparisonTree::read_from_file(const char* file_name)
{
	FILE *f;
	char line[1024];
	const char *sep = " \t";
	long int tree_size;
	ComparisonTree* result = NULL;
	if ((f = fopen(file_name, "r")) != NULL) {
		if (!fgets(line, sizeof(line), f))
		{
			printf("error: a file must contain at least the number of nodes\n");
			return result;
		}
		tree_size = strtoul(line, NULL, 10);
		result = new ComparisonTree(tree_size);

		while (fgets(line, sizeof(line), f) != NULL) {
			unsigned long left_value, right_value;
			char left_type_chr, right_type_chr;
			char opr_chr;

			// read index
			long int index;
			char *p, *token;

			p = strstr(line, "n_") + 2;

			index = strtoul(p, &p, 10);

			// read opr = {+, -, *, /, &, |, <, >, =, [, ]}
			// expecting a white space and then the operand
			token = strtok(p, sep);

			opr_chr = *token;

			// read left type = {n, a, v}
			token = strtok(NULL, sep);
			left_type_chr = *token;	

			// read left value of type unsigned long
			token = strtok(NULL, sep);
			left_value = strtoul(token, NULL, 10);

			// read right type = {n, a, v}
			token = strtok(NULL, sep);
			right_type_chr = *token;

			// read right value of type unsigned long
			token = strtok(NULL, sep);
			right_value = strtoul(token, NULL, 10);

			result->populate_tree(index, left_value, left_type_chr, 
				right_value, right_type_chr, opr_chr);
		}
		fclose(f);
	}
	return result;
}
