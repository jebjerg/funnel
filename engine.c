#include <stdlib.h>
#include <dlfcn.h>
#include "engine.h"

void evaluate_rules(struct context *ctx, struct rule **rules, size_t num_rules) {
	struct rule *rule;
	for (int i = 0; i < num_rules; i++) {
		rule = rules[i];
		if (/* native domain condition */ rule->domain != NULL && !strstr(ctx->url, rule->domain)) {
			DEBUG("rule[%d:%s] DIDNT match: %s\n", i, rule->condition_fp, "native domain condition failed");
			continue;
		}
		if (/* native path condition */ rule->path != NULL && !strstr(ctx->url, rule->path)) {
			DEBUG("rule[%d:%s] DIDNT match: %s\n", i, rule->condition_fp, "native path condition failed");
			continue;
		}
		if (/* module condition */ rule->condition_fp != NULL) {
			int module_ret = evaluate_rule_condition(ctx, rule);
			if (/* module condition */ module_ret != -1 && module_ret != 1) {
				DEBUG("rule[%d:%s] DIDNT match: %s\n", i, rule->condition_fp, "module condition failed");
				continue;
			}
		}
		
		DEBUG("rule[%d] matched\n", i);
		run_rule_handle(ctx, rule);

		if (rule->cont) {
			DEBUG("rule[%d] handler didn't behave (status: %d), continuing\n", i, ctx->status);
			continue;
		}

		/* TODO:
		if (rule->continue_if_failure && ctx->status == CTX_HANDLE_FAILURE) {
			DEBUG("rule[%d] handler failed, continue\n", i);
			continue;
		} else if (rule->continue_if_success && ctx->status == CTX_HANDLE_SUCCESS) {
			DEBUG("rule[%d] handler succeeded, continue\n", i);
			continue;
		} else if (ctx->status == CTX_HANDLE_SUCCESS) {
			DEBUG("rule[%d] handler succeeded, stop\n", i);
			break;
		} else if (ctx->status == CTX_HANDLE_FAILURE) {
			DEBUG("rule[%d] handler failed, stop\n", i);
			break;
		} else if (rule->continue_if_failure) {
			continue;
		} else {
		*/
	}
}

int evaluate_rule_condition(struct context *ctx, struct rule *rule) {
	if (rule->condition_handler == NULL && rule->condition_fp != NULL && rule->condition == NULL) {
		rule->condition_handler = dlopen(rule->condition_fp, RTLD_LAZY);
		if (!rule->condition_handler) {
			goto fail;
		}
		rule->condition = dlsym(rule->condition_handler, (rule->condition_symbol ? rule->condition_symbol : "condition"));
		if (!rule->condition) {
			goto fail;
		}
	}
	if (!rule->condition) {
		goto fail;
	}
	return rule->condition(ctx, NULL);
fail:
	return -1;
}

void run_rule_handle(struct context *ctx, struct rule *rule) {
	if (rule->handler_handler == NULL && rule->handler_fp != NULL && rule->handler == NULL) {
		rule->handler_handler = dlopen(rule->handler_fp, RTLD_LAZY);
		if (!rule->handler_handler) {
			goto fail;
		}
		rule->handler = dlsym(rule->handler_handler, (rule->handler_symbol ? rule->handler_symbol : "handler"));
		if (!rule->handler) {
			goto fail;
		}
	}
	if (!rule->handler) {
		goto fail;
	}
	rule->handler(ctx);
	return;
fail:
	puts("unable to run handler");
	return;
}
