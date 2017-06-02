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

/*#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip_fw.h>
#include <netinet/ip_diffuse.h>
#include <netinet/ipfw/diffuse_classifier_c45.h>
#include "ipfw2.h"
#include "classifier_nbayes.h"
*/
#include "diffuse_classifier_c45.h"


enum classifier_c45_tokens {
	TOK_MODEL_FILE = TOK_FEATURE_MOD_START, 
};


static diffuse_option_t classifier_c45_params[] = {
	{ "model",      DIFFUSE_OPTION_ARG_STR,   0, 0,         TOK_MODEL_FILE },
        /*{ NULL, 0, 0, 0, 0 }*/  /* terminator */
};


int c45_get_cnf_size(void)
{
	/* return fixed size minus pointer */
	return(sizeof(di_classifier_c45_cnf_t));
}


int c45_get_opts(diffuse_option_t **opts)
{
	*opts = classifier_c45_params;
	return sizeof(classifier_c45_params);
}


/* parse function */
int c45_parse_opts(int token, const char *arg_val, struct di_oid *buf)
{
	static di_classifier_c45_cnf_t *cnf = NULL;

	printf("token: %d, name: %s, cnf is %s (%p:%p)\n", token, arg_val, (cnf==NULL) ? "NULL" : "not NULL", cnf, &cnf);

	if (cnf == NULL) {
		cnf = (di_classifier_c45_cnf_t *) buf;
		cnf->model_name[0] = '\0';
		cnf->class_cnt = 0;
		cnf->feature_cnt = 0;
	}

	switch(token) {
        case TOK_OPTS_INIT:
                break;

        case TOK_MODEL_FILE:
		strncpy(cnf->model_name, arg_val, 256-1);
                cnf->model_name[256-1] = '\0';
		printf("before copied : %s\n", arg_val);
		printf("after copied : %s\n", cnf->model_name);
                break;

	default:
		/* this should never happen unless we have a bug */
		errx(EX_DATAERR, "invalid option, fix source");	
	}

	return 0;
}

char *print_id(int id, int type)
{
	static char buf[16];

	if (type & DIFFUSE_C45_CLASS) {
		sprintf(buf, "c_%u", id);
	} else if (type & DIFFUSE_C45_NODE) {
		sprintf(buf, "n_%u", id);
	} else {
		sprintf(buf, "a_%u", id);
	}

	return buf;
}

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
			/* XXX no support for non-binary nominal yet */
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

void c45_print_usage()
{
	printf("algorithm c45 model <file-name>\n");
}


typedef struct c45_id {
	uint16_t val;
	uint8_t type;
} c45_id_t;

c45_id_t parse_id(unsigned int line_no, char *s, int type)
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

int c45_load_model(struct di_oid *buf, char **model, char **feature_str,
		char **class_str)
{
	di_classifier_c45_cnf_t *cnf = (di_classifier_c45_cnf_t *) buf;
	FILE *f;
	char *sep = " \t";
	uint16_t multi = 16; /* multi is 2^multi */
	char *word;
	char line[1024], tmp[1024];
	/* allocate this statically for now */
	static char mod_data[65535], fstr[1024], clstr[1024];
	unsigned int line_no = 0;
	c45_node_real_t *nodes;
	uint16_t max_nodes = sizeof(mod_data)/sizeof(c45_node_real_t);

	if (cnf->model_name == NULL) {
		errx(EX_DATAERR, "no classifier model specified");
	} 

	fstr[0] = '\0';
	clstr[0] = '\0';

	/* the precision */
	cnf->multi = multi;

	/* pointer to list of dists */
	cnf->tree_len = 0;
	nodes = (c45_node_real_t *) mod_data;

	if ((f = fopen(cnf->model_name, "r")) != NULL) {
		while (fgets(line, sizeof(line), f) != NULL) {
			line_no++;

			/* trim leading ws and check that we actually have at least 1 char */
			char *p = line;
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
                                        cnf->class_cnt++;
                                       	strcat(clstr, word);
                                       	strcat(clstr, ",");
                                }
                                /* get rid of last comma */
                                clstr[strlen(clstr)-1] = '\0';
				continue;
                        } else if (!strcmp(word, "attributes")) {
				word = strtok(NULL, sep);
                                for (; word; word = strtok(NULL, sep)) {
                                        cnf->feature_cnt++;
                                       	strcat(fstr, word);
                                       	strcat(fstr, ",");
                                }
                                /* get rid of last comma */
                                fstr[strlen(fstr)-1] = '\0';
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
                                node = (c45_node_real_t *) &nodes[x.val];
                                cnf->tree_len = (x.val + 1) * sizeof(c45_node_real_t);

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
					node->val = v;//(int64_t) round(v);//round(v * (1 << cnf->multi));

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

		fclose(f);
	} else {
		errx(EX_DATAERR, "could not open classifier model %s", cnf->model_name);
	}

	if (cnf->tree_len == 0) {
		errx(EX_DATAERR, "empty classifier model %s", cnf->model_name);
	}

#ifdef DIFFUSE_DEBUG2
	printf("model file: %s\n", cnf->model_name);
	printf("classes: %d\n", cnf->class_cnt);
	printf("attributes: %d\n", cnf->feature_cnt);
	printf("multi: %d\n", (1 << cnf->multi));
	printf("tree_len: %d\n", cnf->tree_len);
	c45_print_model(buf, mod_data);
#endif

	*model = mod_data;
        *feature_str = fstr;
        *class_str = clstr;

	return cnf->tree_len;
}

static struct classifier_module c45_classifier_module = {
	"c4.5",
	.get_cnf_size = c45_get_cnf_size,
        .get_opts = c45_get_opts,
        .parse_opts = c45_parse_opts,
	.print_opts = c45_print_opts,
	.print_usage = c45_print_usage,
	.load_model = c45_load_model,
        /*.name = "c4.5",
        .get_cnf_size = c45_get_cnf_size,
        .get_opts = c45_get_opts,
        .parse_opts = c45_parse_opts,
	.print_opts = c45_print_opts,
	.print_usage = c45_print_usage,
	.load_model = c45_load_model,*/
};


struct classifier_module *c45_module(void)
{
        return &c45_classifier_module;
}


