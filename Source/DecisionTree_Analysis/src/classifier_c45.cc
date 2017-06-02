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
 * $FreeBSD: sbin/ipfw/classifier_c45.c bb68cc04f55e 2011/01/17 02:06:22 szander $
 *
 * Machine learning classsifier and remote action nodes support (DIFFUSE)
 * http://www.caia.swin.edu.au/urp/diffuse
 *
 * Description:
 * C4.5 classifier 
 */

//#include <sys/types.h>
//#include <sys/socket.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
//#include <libutil.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <math.h>

#include <vector>
#include <string>
#include "classifier_c45.h"
#include <iostream>
#include <fstream>

c45_classifier::c45_classifier(const char *model_name)
{
	_model_name.assign(model_name);
	_nodes.resize(100);

	c45_load_model();
}

/*
void c45_print_model(struct di_oid *opts, char *mod_data)
{
	int i;
        di_classifier_c45_cnf_t *cnf = (di_classifier_c45_cnf_t *) opts;
	c45_node_real_t *nodes;
	
	if (mod_data != NULL) {
		nodes = (c45_node_real_t *) mod_data;
	} else {
	 	nodes = (c45_node_real_t *) cnf->nodes;
	}

	for(i=0; i<cnf->tree_len/sizeof(c45_node_real_t); i++) {
		printf("    n_%u a_%u %s c_%u ", i, nodes[i].nid.feature, (nodes[i].nid.type & DIFFUSE_C45_REAL) ? "r" : "n",  
			nodes[i].nid.missing_class);
		if (nodes[i].nid.type == DIFFUSE_C45_BNOM) {
			// XXX no support for non-binary nominal yet
		} else {
			printf("%.2f ", (double)nodes[i].val / (1 << cnf->multi)); 
			printf("%s ", print_id(nodes[i].le_id, nodes[i].le_type));
                        printf("%s\n", print_id(nodes[i].gt_id, nodes[i].gt_type));
		}
	}
}

void c45_print_opts(struct di_oid *opts)
{
	di_classifier_c45_cnf_t *cnf = (di_classifier_c45_cnf_t *) opts;

	printf("  model name: %s\n", cnf->model_name);
	printf("  model:\n");
	c45_print_model(opts, NULL);
}
*/

c45_id_t c45_classifier::parse_id(unsigned int line_no, char *s, int type)
{
	char *p = NULL, *endptr;
	c45_id_t ret = {0, 0};
	uint16_t max_id;

	if (type & DIFFUSE_C45_CLASS) {
		p = strstr(s, "c_");
		ret.type = DIFFUSE_C45_CLASS;
	}
        if (p == NULL && (type & DIFFUSE_C45_NODE)) {
		p = strstr(s, "n_");
		ret.type = DIFFUSE_C45_NODE;
        }
	if (p == NULL && (type & DIFFUSE_C45_FEAT)) {
                p = strstr(s, "a_");
		ret.type = DIFFUSE_C45_FEAT;
        }

	if (p == NULL) {
		errx(EX_DATAERR, "model line %u: invalid class/node/feature id specification %s", line_no, s);
	}

	if (ret.type == DIFFUSE_C45_NODE) {
		max_id = 4095;
	} else {
		max_id = 255;
	} 
	
	p += 2;
	ret.val = atoi(p);
        //ret.val = strtonum(p, 0, max_id, (const char **) &endptr);
        //if (endptr != NULL) {
	if (ret.val < 0 || ret.val > max_id) {
              	errx(EX_DATAERR, "model line %u: invalid or out of range (0--%d) class/node id %s", line_no, max_id, p);
        }

        return ret;
}			

int c45_classifier::c45_load_model()
{
	char *sep = " \t";
	char *word;
	char tmp[1024];
	std::string line;
	unsigned int line_no = 0;
	c45_node_real_t *nodes;
	uint16_t max_nodes = 3640; // (65545)/sizeof(c45_node_real_t);

	printf("actual model_str: |%s|\n", _model_name.c_str());

	if (_model_name == "") {
		errx(EX_DATAERR, "no classifier model specified");
	} 

	/* pointer to list of dists */
	_tree_len = 0;

	std::ifstream f (_model_name);

	if ( f.is_open() ) {
		std::cout << "Input : \n";
		while ( getline(f, line) ) {
			std::cout << line << "\n";
			line_no++;

			/* trim leading ws and check that we actually have at least 1 char */
			const char *p = line.c_str();
			while (isspace(*p)) p++;
			if (!strlen(p) || !sscanf(p, "%[^\r\n]\n", tmp)) {
				continue;
			}

			word = strtok(tmp, sep);

			if (!strncmp(word, "#", 1)) {
                                /* ignore comments */
                                continue;
                        } else if (!strcmp(word, "classes")) {
				word = strtok(NULL, sep);
				for (; word; word = strtok(NULL, sep)) {
					_class_str.push_back(word);
                                }
				continue;
                        } else if (!strcmp(word, "attributes")) {
				word = strtok(NULL, sep);
                                for (; word; word = strtok(NULL, sep)) {
					_feature_str.push_back(word);
                                }
				continue;
                        } else {
				/* tree node description */
				char *endptr = NULL;
                                c45_node_real_t *node = NULL;
				c45_id_t x;

				x = parse_id(line_no, word, DIFFUSE_C45_NODE);
				if (x.val > max_nodes) {
					errx(EX_DATAERR, "classifier model too large (max %u nodes)\n", max_nodes);
				}
				if (x.val >= _nodes.size()) _nodes.resize(x.val+1);
                                node = (c45_node_real_t *) &(_nodes[x.val]);
				printf("val = %d - %p - %p - %p\n", x.val, node, &(_nodes[x.val]), _nodes);
                                _tree_len = (x.val + 1);

				word=strtok(NULL, sep);
				if (!word) {
					goto bad;
				}
				x = parse_id(line_no, word, DIFFUSE_C45_FEAT);
				node->nid.feature = x.val;

				word=strtok(NULL, sep);
				if (!word) {
					goto bad;
				}
				if (!strcmp(word, "r")) {
					node->nid.type = DIFFUSE_C45_REAL;
				} else if (!strcmp(word, "n")) {
					node->nid.type = DIFFUSE_C45_BNOM;
					/* XXX check if non-binary */
				} else {
					errx(EX_DATAERR, "model line %u: unknown node type %s", line_no, word);
				}

				word=strtok(NULL, sep);
				if (!word) {
					goto bad;
				}
				x = parse_id(line_no, word, DIFFUSE_C45_CLASS);
                                node->nid.missing_class = x.val;	

				/* parse split values and classes */
				word = strtok(NULL, sep);
				if (!word) {
					goto bad;
				}
				if (node->nid.type == DIFFUSE_C45_NOM) {
					/* XXX non-binary nominal not supported yet */
					errx(EX_DATAERR, "model line %u: non-binary nominal splits not supported yet", line_no);

				} else {
					/* binary nominal has the same structures as real */
					double v = strtod(word, &endptr);
                                        if (endptr == NULL) {
                                        	errx(EX_DATAERR, "model line %u: split value not a number %s", line_no, word);
                                        }
					node->val = v;

					word = strtok(NULL, sep);
					if (!word) {
						goto bad;
					}
					x = parse_id(line_no, word, DIFFUSE_C45_NODE | DIFFUSE_C45_CLASS);
					node->le_id = x.val;
					node->le_type = x.type;

					word = strtok(NULL, sep);
					if (!word) {
                                                goto bad;
                                        }
					x = parse_id(line_no, word, DIFFUSE_C45_NODE | DIFFUSE_C45_CLASS);
					node->gt_id = x.val;
					node->gt_type = x.type;
				}
			}
bad:
			if (!word) {
				errx(EX_DATAERR, "model line %u: missing value(s)", line_no);
			}
		}

		f.close();
	} else {
		errx(EX_DATAERR, "could not open classifier model %s", _model_name.c_str());
	}

	if (_tree_len == 0) {
		errx(EX_DATAERR, "empty classifier model %s", _model_name.c_str());
	}

	return _tree_len;
}

int c45_classifier::c45_classify(std::vector<double> features)
{
	int n = 0;
	int fcnt = features.size();
	double fval;

	while (n < _tree_len) {
		c45_node_template_t* node_tmp = &_nodes[n];
		c45_node_real_t* node = (c45_node_real_t*) node_tmp;
		if (node->nid.feature > fcnt - 1) {
			/* should never happen */
			return -1;
		}

		fval = features[node->nid.feature];
		
		switch (node->nid.type) {
		case DIFFUSE_C45_REAL:
			if (fval <= node->val) {
				if (node->le_type & DIFFUSE_C45_CLASS) {
					return node->le_id;
				} else {
					n = node->le_id;
				}
			} else {
				if (node->gt_type & DIFFUSE_C45_CLASS) {
                                	return node->gt_id;
                        	} else {
                                	n = node->gt_id;
                        	}
			}
			break;
		case DIFFUSE_C45_BNOM:
			//printf("diffuse_classifier_c45.cc: %lf %lf %d %d \n", fval, nodes[n].val, nodes[n].le_id, nodes[n].gt_id);
			if (fval == node->val) {
                                if (node->le_type & DIFFUSE_C45_CLASS) {
                                        return node->le_id;
                                } else {
                                        n = node->le_id;
                                }
                        } else {
                                if (node->gt_type & DIFFUSE_C45_CLASS) {
                                        return node->gt_id;
                                } else {
                                        n = node->gt_id;
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
