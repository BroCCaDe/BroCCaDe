/*
 * Copyright (c) 2010-2011 Centre for Advanced Internet Architectures,
 * Swinburne University of Technology. 
 *
 * Author: Sebastian Zander (szander@swin.edu.au)
 *
 * Redistribution and use in source forms, with and without modification,
 * are permitted provided that this entire comment appears intact.
 *
 * Redistribution in binary form may occur without any restrictions.
 * Obviously, it would be nice if you gave credit where credit is due
 * but requiring it would be too onerous.
 *
 * This software is provided ``AS IS'' without any warranties of any kind.
 *
 * $FreeBSD: sbin/ipfw/classifier_c45.h 79b492662e6a 2010/11/16 00:14:03 szander $
 *
 * Machine learning classsifier and remote action nodes support (DIFFUSE)
 * http://www.caia.swin.edu.au/urp/diffuse
 *
 * Description:
 * C4.5 classifier 
 */

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

#ifndef _CLASSIFIER_C45_H
#define _CLASSIFIER_C45_H

#include <string>
#include <vector>

typedef struct c45_id {
	uint16_t val;
	uint8_t type;
} c45_id_t;

/* bit flags for id type */
enum c45_id_types {
	DIFFUSE_C45_CLASS  = 0x01,
	DIFFUSE_C45_NODE   = 0x02,
	DIFFUSE_C45_FEAT   = 0x04
};

/* bit flags for value split */
enum c45_val_types {
        DIFFUSE_C45_BNOM  = 0x01,
        DIFFUSE_C45_NOM   = 0x02,
        DIFFUSE_C45_REAL  = 0x03
};

/* NOTE: number of classes, features limited to 256 */

typedef struct c45_node {
	/* type of node, split node vs leaf 
           type of value, nominal vs real */
	uint16_t type;
	/* feature number */
	uint8_t feature;
	/* class if feature missing */
	uint8_t missing_class;
} c45_node_t;

typedef struct c45_node_template {
	c45_node_t nid;
	
	double val;
	
	// this is a template for the 2 short and 2 char for ID and types
	char data[6];
} c45_node_template_t;

typedef struct c45_node_real {
        c45_node_t nid;

	/* split value */
	//int64_t val;
	double val;
	/* le = less-equal class (class or node depending on type) 
           ge = greater-than class (class or node depending on type) */
	uint16_t le_id;
	uint16_t gt_id;
	uint8_t le_type;
	uint8_t gt_type;
} c45_node_real_t;

typedef struct c45_node_bin_nominal {
	c45_node_t nid;
	
	/* value */
	//int64_t val;
	double val;
	/* eq = equal class (class or node depending on type) 
           ne = non-equal class (class or node depending on type) */
        uint16_t eq_id;
	uint16_t ne_id;
        uint8_t eq_type;
        uint8_t ne_type;
} c45_node_bin_nominal_t;

class c45_classifier
{
public:
	c45_classifier(const char* model_file);
	~c45_classifier() {};
	
	void c45_print_model();
	int c45_classify(std::vector<double> features);
	unsigned int get_feature_length() {return _feature_str.size();}
	std::string get_feature_str(int index) {return _feature_str[index];}
private:
	c45_id_t parse_id(unsigned int line_no, char *s, int type);
	int c45_load_model();

	std::vector<c45_node_template_t> _nodes;
	std::vector<std::string> _feature_str;
	std::vector<std::string> _class_str;
	std::string _model_name;
	uint16_t _tree_len;
};

#endif /* _CLASSIFIER_C45_H */
