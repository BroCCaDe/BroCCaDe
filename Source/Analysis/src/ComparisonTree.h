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

#ifndef AUX_PLUGINS_COMPARISONTREE_H
#define AUX_PLUGINS_COMPARISONTREE_H

#include <vector>
#include <malloc.h>
#include <stdio.h>

class CT_Enum
{
public:
	enum Node_Type {
		NODE_OPERATION,
		NODE_FEATURE_INDEX,
		NODE_VALUE,
		NODE_INVALID
	};

	enum Operation_Type {
		OPERATION_PLUS,
		OPERATION_MINUS,
		OPERATION_TIMES,
		OPERATION_DIVIDE,
		OPERATION_AND,
		OPERATION_OR,
		OPERATION_GT,
		OPERATION_GE,
		OPERATION_LT,
		OPERATION_LE,
		OPERATION_EQ,
		OPERATION_INVALID
	};

	static const char* node_type_to_str(Node_Type type);
	static const char* opr_to_str(Operation_Type opr);
	static Node_Type switch_type(const char c);
	static Operation_Type switch_opr(const char c);
	static unsigned int binary_opr(unsigned int lval, unsigned int rval, 
		Operation_Type type);
};

class CT_Node
{
public:

	CT_Node(unsigned int left_value, CT_Enum::Node_Type left_type, 
		unsigned int right_value, CT_Enum::Node_Type right_type, 
		CT_Enum::Operation_Type opr)
	{
		_right_value = right_value;
		_right_type = right_type;
		_left_value = left_value;
		_left_type = left_type;
		_opr = opr;
		printf("(%u, %s) %s (%u, %s)\n", 
			left_value, CT_Enum::node_type_to_str(left_type),
			CT_Enum::opr_to_str(opr), 
			right_value, CT_Enum::node_type_to_str(right_type));
	}

	~CT_Node() {};
	unsigned int get_right_value() {return _right_value;}
	CT_Enum::Node_Type get_right_type() {return _right_type;}
	unsigned int get_left_value() {return _left_value;}
	CT_Enum::Node_Type get_left_type() {return _left_type;}
	CT_Enum::Operation_Type get_operation_type() {return _opr;}
private:
	unsigned int _right_value;
	CT_Enum::Node_Type _right_type;
	unsigned int _left_value;
	CT_Enum::Node_Type _left_type;
	CT_Enum::Operation_Type _opr;
};

class ComparisonTree
{
public:
	ComparisonTree(unsigned int node_count) 
	{
		_tree = (CT_Node**) malloc (sizeof (CT_Node*) * node_count);
		_node_count = node_count;
	}

	~ComparisonTree() {if (_tree) delete[] _tree;} // TODO
	
	static ComparisonTree* read_from_file(const char* file_name);

	void populate_tree(unsigned int index, unsigned int left_value, char left_type_chr, 
		unsigned int right_value, char right_type_chr, char opr_chr);

	CT_Node* get_root() {return _tree[0];}
	unsigned int calculate(std::vector<unsigned int> features);
private:
	unsigned int get_result(std::vector<unsigned int> features, 
		CT_Enum::Node_Type type, unsigned int value);
	unsigned int calculate_rec(std::vector<unsigned int> features, 
		CT_Node* _current_tree);
	CT_Node** _tree;
	unsigned int _node_count;
};
#endif
