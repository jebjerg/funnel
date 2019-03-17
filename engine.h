#ifndef ENGINE_H
#define ENGINE_H

#include "funnel.h"
#include "rules.h"

struct engine {
	char *rules_fn;
	size_t num_rules;
	struct rule **ruleset;
};

struct engine *create_engine(char *rules_fn);
void destroy_engine(struct engine *engine);
int engine_load_rules(struct engine *engine);
void engine_evaluate(struct engine *engine, struct context *ctx);
int evaluate_rule_condition(struct context *ctx, struct rule *rule);
void run_rule_handle(struct context *ctx, struct rule *rule);
void run_rule_handle(struct context *ctx, struct rule *rule);

#endif /* ENGINEE_H */
