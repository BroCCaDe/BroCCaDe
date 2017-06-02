/*-
 * Copyright (c) 2010-2011 Centre for Advanced Internet Architectures,
 * Swinburne University of Technology. 
 *
 * Author: Sebastian Zander (szander@swin.edu.au)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * DIFFUSE C4.5 classifier 
 *
 * $FreeBSD: sys/netinet/ipfw/diffuse_classifier_c45.h d900ca0a48d0 2011/03/21 05:29:55 szander $
 */

#ifndef _DIFFUSE_CLASSIFIER_C45_H
#define _DIFFUSE_CLASSIFIER_C45_H

/*#ifdef _KERNEL
#include <netinet/ip_diffuse.h>
#endif
#include <netinet/ipfw/diffuse_classifier.h>*/
#include "diffuse_classifier.h"


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

/* XXX no support for non-binary nominal yet */

typedef struct di_classifier_c45_cnf
{
	struct di_oid oid;

	/* model name */
	char model_name[256];
	/* number of features */
	uint16_t feature_cnt;
	/* number of classes */
	uint16_t class_cnt;
	/* precsion, multipler for double->int */
	uint16_t multi;
	/* length of nodes */
	uint16_t tree_len;
	/* tree */
	c45_node_t nodes[];
} di_classifier_c45_cnf_t;


int c45_classify(struct cdata *cdata, double *features, int fcnt);


#endif /* _DIFFUSE_CLASSIFIER_C45_H */
