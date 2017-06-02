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
 * The API to write a classifier module for DIFFUSE.
 *
 * $FreeBSD: sys/netinet/ipfw/diffuse_classifier.h 546a45450246 2011/01/18 08:11:15 szander $
 */

#ifndef _DIFFUSE_CLASSIFIER_H
#define _DIFFUSE_CLASSIFIER_H

/*#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <netinet/ip_diffuse.h>
#include <netinet/ipfw/diffuse_feature.h>*/ /* struct cdata */
#include "diffuse.h"
#include "queue.h"


/*
 * Descriptor for a classifier. Contains all function pointers 
 * for a given classifier 
 * This is typically created when a module is loaded, and stored
 * in a global list of classifiers.
 */
struct diffuse_classifier_alg {
	const char *name;   /* classifier name */

	/* init instance 
           param1: pointer to instance config
	   param2: config from userspace
	*/
	int (*init_instance)(struct cdata*, struct di_oid*);

	/* destroy instance
	   param1: pointer to instance config
	*/
 	int (*destroy_instance)(struct cdata*);

        /* classify packet (sub flow) 
           param1: pointer to instance config 
           param2: pointer to features 
	   param3: number of features
	   return: class
        */
        int (*classify)(struct cdata*, double*, int);

	/* get configuration data
	   param1: pointer to instance config
	   param2: pointer to configuration
           param3: only compute size (if 1) 
	   return: number of stats 
	*/
	int (*get_conf)(struct cdata*, struct di_oid*, int);

	/* get number of features needed
	   return: number of features 
	*/
	int (*get_feature_cnt)(struct cdata*);

	/* get number of classes 
           return: number of classes 
        */
        int (*get_class_cnt)(struct cdata*);


        int ref_count;      /* number of instances in the system */
        SLIST_ENTRY(diffuse_classifier_alg) next; /* Next feature in the list */

};

#ifdef _KERNEL

/* MSVC does not support initializers so we need this ugly macro */
#ifdef _WIN32
#define _CI(fld)
#else
#define _CI(fld)	fld
#endif

int diffuse_classifier_modevent(module_t mod, int cmd, void *arg);

#define DECLARE_DIFFUSE_CLASSIFIER_MODULE(name, classifier)	\
	static moduledata_t name##_mod = {			\
		#name, diffuse_classifier_modevent, classifier	\
	};							\
	DECLARE_MODULE(name, name##_mod, 			\
		SI_SUB_PROTO_IFATTACHDOMAIN, SI_ORDER_ANY); 	\
        MODULE_DEPEND(name, diffuse, 1, 1, 1);

#endif

#endif /* _DIFFUSE_CLASSIFIER_H */
