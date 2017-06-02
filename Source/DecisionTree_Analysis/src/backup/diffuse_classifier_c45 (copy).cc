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
 * $FreeBSD: sys/netinet/ipfw/diffuse_classifier_c45.c b7535e2e8498 2011/01/18 05:59:30 szander $
 */

/*#ifdef _KERNEL
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/mbuf.h>
#else
#include <sys/types.h>
#include <stdio.h>
#endif
#include <sys/socket.h>
#include <net/if.h> */    /* IFNAMSIZ */
/*#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip_var.h>
#ifdef _KERNEL
#include <netinet/ipfw/diffuse_private.h>
#endif
#include <netinet/ip_diffuse.h>*/
//#include <netinet/ipfw/diffuse_classifier_c45.h>
#include "diffuse_classifier_c45.h"
#include <string>
/*
 * C4.5 classifier 
 */

#ifdef _KERNEL

/* computes the buffer size needed to store classifier */
static int get_size(di_classifier_c45_cnf_t *c)
{
	return sizeof(di_classifier_c45_cnf_t) + c->tree_len;
}

/* assumes target memory is allocated */
static void cpy_cnf(di_classifier_c45_cnf_t *f, di_classifier_c45_cnf_t *t)
{
	t->oid = f->oid;
	strcpy(t->model_name, f->model_name); 
	t->feature_cnt = f->feature_cnt;
	t->class_cnt = f->class_cnt;
	t->multi = f->multi;
	t->tree_len = f->tree_len;
	memcpy(t->nodes, f->nodes, f->tree_len);	
	
} 

static int 
c45_init_instance(struct cdata *cdata, struct di_oid *params)
{
	di_classifier_c45_cnf_t *c = (di_classifier_c45_cnf_t *) params;
	di_classifier_c45_cnf_t *cnf;

	DID("class cnt %d", c->class_cnt);
	DID("attr cnt %d", c->feature_cnt);
	DID("multi %d", (1 << c->multi));
	DID("tree_len %d", c->tree_len);
	DID("want size %d", get_size(c));

	cdata->conf = malloc(get_size(c), M_DIFFUSE, M_NOWAIT | M_ZERO);
	cnf = (di_classifier_c45_cnf_t *) cdata->conf;
	cpy_cnf(c, cnf);

	return 0;
}


static int
c45_destroy_instance(struct cdata *cdata)
{
	free(cdata->conf, M_DIFFUSE);

	return 0;
}

static int 
c45_get_conf(struct cdata *cdata, struct di_oid *cbuf, int size_only)
{
	int len = get_size((di_classifier_c45_cnf_t *) cdata->conf);

	if (!size_only) {
		di_classifier_c45_cnf_t *cnf = (di_classifier_c45_cnf_t *) cdata->conf;
		di_classifier_c45_cnf_t *rcnf = (di_classifier_c45_cnf_t *) cbuf;
		cpy_cnf(cnf, rcnf);
	}

	return len;
}

#endif

int
c45_classify(struct cdata *cdata, double *features, int fcnt)
{
	di_classifier_c45_cnf_t *cnf  = (di_classifier_c45_cnf_t*) cdata->conf;
	int n = 0;
        c45_node_real_t *nodes = (c45_node_real_t *) cnf->nodes;
	double fval;

#ifdef DIFFUSE_DEBUG2
	printf("diffuse: %-10s features ", __FUNCTION__);
	for (int i=0; i<fcnt; i++) {
		printf("%lf ", features[i]);
	}
	printf("\n");
#endif

	while (n < cnf->tree_len/sizeof(c45_node_real_t)) {
		if (nodes[n].nid.feature > fcnt - 1) {
			/* should never happen */
			return -1;
		}

		fval = features[nodes[n].nid.feature];// * (1 << cnf->multi);
		
		switch (nodes[n].nid.type) {
		case DIFFUSE_C45_REAL:
			if (fval <= nodes[n].val) {
				if (nodes[n].le_type & DIFFUSE_C45_CLASS) {
					return nodes[n].le_id;
				} else {
					n = nodes[n].le_id;
				}
			} else {
				if (nodes[n].gt_type & DIFFUSE_C45_CLASS) {
                                	return nodes[n].gt_id;
                        	} else {
                                	n = nodes[n].gt_id;
                        	}
			}
			break;
		case DIFFUSE_C45_BNOM:
			printf("diffuse_classifier_c45.cc: %lf %lf %d %d \n", fval, nodes[n].val, nodes[n].le_id, nodes[n].gt_id);
			if (fval == nodes[n].val) {
                                if (nodes[n].le_type & DIFFUSE_C45_CLASS) {
                                        return nodes[n].le_id;
                                } else {
                                        n = nodes[n].le_id;
                                }
                        } else {
                                if (nodes[n].gt_type & DIFFUSE_C45_CLASS) {
                                        return nodes[n].gt_id;
                                } else {
                                        n = nodes[n].gt_id;
                                }
                        }
			break;
		case DIFFUSE_C45_NOM:
			/* XXX not supported yet */
			return -1;
			break;
		default:
			break;
		} 
	}

	return -1;
}

#ifdef _KERNEL

static int
c45_get_feature_cnt(struct cdata *cdata)
{
	di_classifier_c45_cnf_t *cnf  = (di_classifier_c45_cnf_t*) cdata->conf;

	return cnf->feature_cnt;
}

static int
c45_get_class_cnt(struct cdata *cdata)
{
        di_classifier_c45_cnf_t *cnf  = (di_classifier_c45_cnf_t*) cdata->conf;

        return cnf->class_cnt;
}


/*
 * naive bayes classifier descriptor
 * contains the name and function pointers 
 */

static struct diffuse_classifier_alg diffuse_c45_desc = {
	_FI( .name = ) "c4.5",
	_FI( .ref_count = ) 0,

	_FI( .init_instance = ) c45_init_instance,
	_FI( .destroy_instance = ) c45_destroy_instance,
	_FI( .get_conf = )  c45_get_conf,
	_FI( .classify = )  c45_classify,
	_FI( .get_feature_cnt = ) c45_get_feature_cnt,
	_FI( .get_class_cnt = ) c45_get_class_cnt,
};

DECLARE_DIFFUSE_CLASSIFIER_MODULE(classifier_c45, &diffuse_c45_desc);

#endif
