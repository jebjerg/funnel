#ifndef ENGINE_H
#define ENGINE_H

#include "funnel.h"
#include "rules.h"

void evaluate_rules(struct context *ctx, struct rule **rules, size_t num_rules);
int evaluate_rule_condition(struct context *ctx, struct rule *rule);
void run_rule_handle(struct context *ctx, struct rule *rule);

#endif /* ENGINEE_H */
