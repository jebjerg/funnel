#include <getopt.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "rules.h"

/*
 * -A <queue> -d <domain.tld> -p strncmp(/some/prefix) -j MODULE [-f|-s]
 *  matches domain (-d) and path (-p), "jumping" to module's handler
 *  proceedes (-f) to next rule if handler fails
 *  proceedes (-p) to next rule regardless of handler result
 *
 * -A <queue> -m MODULE
 * -A <queue> -m reddit
 *  uses module "reddit" and its condition function pointer
 *
 * -A <queue> -m reddit -d redd.it
 *  can be used in combination, i.e. handle some filtering in native (simple)
 *  rule engine, and some in module specific engine(s)
 *
 *  -A <queue> -d gfycat.com -j DROP
 */

static struct option rule_opts[] = {
	{ "handler", required_argument, NULL, 'e' },
	{ "condition", required_argument, NULL, 'm' },
	{ "path", required_argument, NULL, 'p' },
	{ "domain", required_argument, NULL, 'd' },
	{ "continue", no_argument, NULL, 'c' },
	{ NULL, 0, NULL, 0}
};

struct rule *parse_rule(char *input, size_t sz) {
	struct rule *rule = malloc(sizeof(*rule));
	memset(rule, 0, sizeof(*rule));
	char *copy = input;


	char *tokens[1024];
	memset(tokens, 0, sizeof(tokens));
	size_t num_tokens = 0;
	char *token;
	tokens[num_tokens++] = "rule";

	while ((token = strsep(&copy, " ")) != 0) {
		tokens[num_tokens++] = token;
	}

	int opt;
	opterr = 0;
	optind = 0;
	while ((opt = getopt_long(num_tokens, tokens, "m:e:p:d:c", rule_opts, NULL)) != -1) {
		switch (opt) {
			case 'c':
				rule->cont = 1;
				break;
			case 'd':
				rule->domain = strdup(optarg);
				break;
			case 'p':
				rule->path = strdup(optarg);
				break;
			case 'e':
				if (strstr(optarg, "#")) {
					rule->handler_fp = strdup(strtok(optarg, "#"));
					rule->handler_symbol = strdup(strtok(NULL, "#"));
				} else {
					rule->handler_fp = strdup(optarg);
					rule->handler_symbol = NULL;
				}
				break;
			case 'm':
				if (strstr(optarg, "#")) {
					rule->condition_fp = strdup(strtok(optarg, "#"));
					rule->condition_symbol = strdup(strtok(NULL, "#"));
				} else {
					rule->condition_fp = strdup(optarg);
					rule->condition_symbol = NULL;
				}
				break;
			default:
				goto fail;
		}
	}
	goto out;
fail:
	DEBUG("error while parsing rule line: %s\n", copy);
	if (rule != NULL) {
		free(rule);
	}
	rule = NULL;
out:
	return rule;
}

void strip_line(char *line, size_t sz) {
	if (line[sz-1] == '\n')
		line[sz-1] = 0;
}

// TODO..
int parse_config(char *fn, struct rule **ruleset, size_t *num_rules) {
	FILE *rules_f = fopen(fn, "r");
	if (!rules_f) {
		return 1;
	}
	char *line = NULL;
	size_t len = 0;
	ssize_t n;

	memset(ruleset, 0, sizeof(*ruleset));
	*num_rules = 0;

	while ((n = getline(&line, &len, rules_f)) != -1) {
		strip_line(line, n);
		struct rule *rule = parse_rule(line, n-1);
		if (rule != NULL) {
			ruleset[(*num_rules)++] = rule;
		}
	}
	free(line);
	fclose(rules_f);
	return 0;
}

void destroy_rules(struct rule **ruleset, size_t num_rules) {
	for (size_t i = 0; i < num_rules; i++) {
		struct rule *rule = ruleset[i];

		if (rule->domain)
			free(rule->domain);
		if (rule->path)
			free(rule->path);
		if (rule->handler_fp)
			free(rule->handler_fp);
		if (rule->handler_symbol)
			free(rule->handler_symbol);
		if (rule->condition_fp)
			free(rule->condition_fp);
		if (rule->condition_symbol)
			free(rule->condition_symbol);

		if (rule->handler_handler != NULL) {
			dlclose(rule->handler_handler);
		}
		if (rule->condition_handler != NULL) {
			dlclose(rule->condition_handler);
		}
		free(rule);
	}
	free(ruleset);
}
