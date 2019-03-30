#include <string.h>
#include <stdio.h>
#include <dlfcn.h>

#include "funnel.h"

int condition(struct context *ctx, void *parameters) {
	puts("reddit condition()");
	return strstr(ctx->url, "reddit.com") != NULL;
}

int condition_v2(struct context *ctx, void *parameters) {
	puts("reddit condition v2()");
	return strstr(ctx->url, "reddit.com") != NULL;
}

void handler(struct context *ctx) {
	puts("reddit handler!");
	ctx->status = CTX_HANDLE_SUCCESS;
}

void handler_v2(struct context *ctx) {
	puts("new reddit handler!");
	ctx->status = CTX_HANDLE_SUCCESS;
}

void handler_shorturl(struct context *ctx) {
	submit_url("http://reddit.com/#fixed");
	ctx->status = CTX_HANDLE_SUCCESS;
}
