#ifndef FUNNEL_H
#define FUNNEL_H

enum context_status {
	CTX_NONE,
	CTX_HANDLE_SUCCESS,
	CTX_HANDLE_FAILURE,
	CTX_RETRY,
};

struct context {
	char *url;
	int status;
};

/*
 * modules should export both handler and condition functions.
 * the mix'n'match is supported for special cases where sharing of multiple
 * handlers or conditions makes sense (i.e. useful abstractions)
 *
 * strive to keep it simple, e.g.:
 */

// module functions
int condition(struct context *ctx, void *parameters);
void handler(struct context *ctx);

// exported functions (by engine owner) intended for modules
int submit_url(char *url); /* (re)submit url for rule matching */
int submit_context(struct context *ctx); /* (re)submit context */

#ifdef DEBUG
#	undef DEBUG
#	define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#	define DEBUG(...)
#endif

#define FREE(ptr) 		\
{				\
	if ((ptr) != NULL) {	\
		free(ptr);	\
		ptr = NULL;	\
	}			\
}

#endif /* FUNNEL_H */
