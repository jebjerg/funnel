#ifndef RULE_H
#define RULE_H

#include <stdio.h>
#include <string.h>

#include "funnel.h"

struct rule {
	/* -j <handler_fp>[.<handler_symbol>] */
	void *handler_handler;
	char *handler_fp;
	char *handler_symbol;
	void (*handler)(struct context *);

	/* -m <condition_fp>[.<condition_symbol>] */
	void *condition_handler;
	char *condition_fp;
	char *condition_symbol;
	int (*condition)(struct context *, void *parameters); /* TODO: should
							       parameters be
							       void *data, or
							       structured?
							       */
	// native match engine
	char *domain;
	char *path;
	int cont;
};

struct rule *parse_rule(char *input, size_t sz);
int parse_config(char *fn, struct rule **ruleset, size_t *num_rules);
void destroy_rules(struct rule **ruleset, size_t num_rules);

#endif /* RULE_H */
