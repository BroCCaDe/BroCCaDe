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

#ifndef _CLASSIFIER_C45_H
#define _CLASSIFIER_C45_H

//#include <netinet/ip_diffuse.h>
#include "diffuse.h"


struct classifier_module *c45_module(void);


#endif /* _CLASSIFIER_C45_H */
