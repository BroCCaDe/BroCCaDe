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
 * $FreeBSD: sbin/ipfw/diffuse.h eb9985355180 2011/01/13 03:37:19 szander $
 *
 * Machine learning classsifier and remote action nodes support (DIFFUSE)
 * http://www.caia.swin.edu.au/urp/diffuse
 */

#ifndef _DIFFUSE_H
#define _DIFFUSE_H

//#include <sys/types.h>
//#include <sys/socket.h>
/* XXX there are several sysctl leftover here */
//#include <sys/sysctl.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
//#include <libutil.h>
//#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

/*#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip_fw.h>
#include <netinet/ip_diffuse.h>
#include <arpa/inet.h>*/	/* inet_ntoa */
#include "tcc_glue.h"

#ifndef MAX_NAME_STR_LEN
#define MAX_NAME_STR_LEN 8 
#endif

struct cdata {
	/* pointer to instance configuration */
        void *conf;
};

struct di_oid {
	uint32_t	len;	 /* total obj len including this header 
				    16 bit too small for flowtable show */
	uint32_t	id;	 /* generic id */
	uint8_t         type;    /* type, e.g. delete or show */
        uint8_t         subtype; /* object, e.g. feature, classifier */ 
	uint16_t	flags;	 /* data we can pass in the oid */
};

/* for what the actions (e.g. config, delete, show) are for */
enum diffuse_types {
	DIFFUSE_FEATURE = 1,
	DIFFUSE_CLASSIFIER,
	DIFFUSE_EXPORT,
	DIFFUSE_FLOWTABLE
};


/* type of arguments */
typedef enum diffuse_option_arg_types {
	DIFFUSE_OPTION_ARG_NOARG = 0,
	DIFFUSE_OPTION_ARG_STR,
	DIFFUSE_OPTION_ARG_CHAR,
	DIFFUSE_OPTION_ARG_INT,
	DIFFUSE_OPTION_ARG_UINT,
	DIFFUSE_OPTION_ARG_DOUBLE
} diffuse_option_arg_types_t;


/* definition of an option */
typedef struct diffuse_option {
        char *name;
        diffuse_option_arg_types_t arg_type;
        double arg_min;
        double arg_max;
        int token;
} diffuse_option_t;

/* for listing flow table */
typedef struct feature_arr_entry {
        char name [MAX_NAME_STR_LEN];
        char mod_name [MAX_NAME_STR_LEN];
} feature_arr_entry_t;


#define TOK_FEATURE_MOD_START 256
/* tokens used by the basic diffuse rules */
enum diffuse_tokens {
        /* TOK_NULL=0, */ /* in ipfw2.h */
        TOK_MODULE = 1,
	TOK_ALGORITHM,
	TOK_USE_FEATURE_STATS,
	TOK_CLASS_NAMES,
	TOK_EXPIRED,
	TOK_EXP_TARGET,
	TOK_EXP_LIMIT,
	TOK_EXP_FLOW_LIMIT,
	TOK_CONFIRM,
	TOK_EXP_FLOW_KEY,
	TOK_EXP_FEATURES,
	TOK_EXP_ACTION,
	TOK_EXP_ACTION_PARAMS,
	TOK_EXP_MIN_BATCH,
	TOK_EXP_MAX_BATCH,
	TOK_EXP_MAX_DELAY,
	TOK_EXP_ACTION_UNIDIR,
	TOK_OPTS_INIT /* used for initialising module options */
};



/* external init function
   param1: pointer to pointer to options
   return: size of returned options in byte
*/
typedef int (*get_options_fct_t)(diffuse_option_t **opts);

/* external parse function 
   param1: token
   param2: arg value (can be NULL)
   param3: pointer to record buffer */
typedef int (*parse_opts_fct_t)(int, const char*, struct di_oid*);

/* returns size of config record

*/
typedef int (*get_config_size_t)(void);

/* prints options
   param1: pointer to option record
*/
typedef void (*print_opts_fct_t)(struct di_oid*);

/* print usage
*/
typedef void (*print_usage_fct_t)(void);

/* get stat name
   param1: stat number
   return: stat name
 */
typedef char* (*get_stat_name_fct_t)(int);


typedef struct feature_module
{
	char name[MAX_NAME_STR_LEN];
	int type;
	get_config_size_t get_cnf_size;
	get_options_fct_t get_opts;
	parse_opts_fct_t parse_opts;
	print_opts_fct_t print_opts;
	print_usage_fct_t print_usage;
  	get_stat_name_fct_t get_stat_name;	
} feature_module_t;


typedef struct feature_module_entry
{
        feature_module_t *mod;
        struct feature_module_entry *next;
} feature_module_entry_t;


/* load model
   param1: pointer to config buffer
   param2: pointer to model loaded 
   param3: pointer to feature name string 
   param4: pointer to class name string 
   return: model size, <0 if error 
 */
typedef int (*load_model_fct_t)(struct di_oid*, char**,
	char**, char**); 

typedef struct classifier_module
{
        char name[MAX_NAME_STR_LEN];
        get_config_size_t get_cnf_size;
        get_options_fct_t get_opts;
        parse_opts_fct_t parse_opts;
        print_opts_fct_t print_opts;
        print_usage_fct_t print_usage;
        load_model_fct_t load_model;
} classifier_module_t;


typedef struct classifier_module_entry
{
	classifier_module_t *mod;
	struct classifier_module_entry *next;
} classifier_module_entry_t;


/* functions */

//void diffuse_config(int ac, char **av, int type);
//void diffuse_show(int ac, char **av, int type, int counters);
//void diffuse_list(int ac, char *av[], int type, int show_counters);
//void diffuse_flush(int ac, char *av[], int reset_counters_only);
//void diffuse_delete(int ac, char *av[], int type);
/* called by ipfw for unknown opcodes */
//int diffuse_show_cmd(ipfw_insn *cmd);

/* parse methods for rule extensions */
//int diffuse_parse_action(int token, ipfw_insn *action, char **av[]);
//int diffuse_parse_cmd(int token, int open_par, ipfw_insn **cmd, char **av[]);

/* called when building the rule */
//void diffuse_rule_build_1(uint32_t cmdbuf[], ipfw_insn *cmd, ipfw_insn **dst);
//void diffuse_rule_build_2(uint32_t cmdbuf[], ipfw_insn *cmd, ipfw_insn **dst);

/* diffuse_modules.c */
int register_classifier_module(classifier_module_entry_t **list, classifier_module_t *c);
classifier_module_t *find_classifier_module(classifier_module_entry_t *list, const char *name);
void deregister_classifier_modules(classifier_module_entry_t *list);
void print_classifier_modules(classifier_module_entry_t *list);

//int register_feature_module(feature_module_entry_t **list, feature_module_t *c);
//feature_module_t *find_feature_module(feature_module_entry_t *list, const char *name);
//void deregister_feature_modules(feature_module_entry_t *list);
//void print_feature_modules(feature_module_entry_t *list);


#endif /* _DIFFUSE_H */
