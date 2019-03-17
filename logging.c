#include <string.h>
#include <stdio.h>

#include "funnel.h"

void handler(struct context *ctx) {
	printf("received %s\n", ctx->url);
	// ctx->status = CTX_HANDLE_SUCCESS;
}
