#include <string.h>
#include <stdio.h>

#include "funnel.h"

void handler(struct context *ctx) {
	puts("LOGGING!");
	// ctx->status = CTX_HANDLE_SUCCESS;
}
