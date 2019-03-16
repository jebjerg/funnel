#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include "funnel.h"
#include "rules.h"
#include "engine.h"

#define RULES_MAX 100
int running = 1;

void sighandler(int num) {
	switch (num) {
		case SIGINT:
			exit(1);
			// TODO:
			running = 0;
			break;
		case SIGHUP:
			// reload config
			break;
		default:
			break;
	}
}

int main(int argc, char **argv) {
	/*

	puts(buf);
	return 1;
	*/
	struct rule **ruleset = malloc((sizeof *ruleset) * 100);
	size_t num_rules = 0;

	if (parse_config("rules.txt", ruleset, &num_rules)) {
		DEBUG("aww");
		goto out;
	}

	if (ruleset == NULL || num_rules == 0) {
		DEBUG("no rules");
		goto out;
	}

	DEBUG("loaded %ld rules\n", num_rules);

	struct context ctx = {
		.url = "https://reddit.com/foo.png",
		.status = CTX_NONE,
	};
	evaluate_rules(&ctx, ruleset, num_rules);

	/*
	signal(SIGINT, sighandler);
	signal(SIGHUP, sighandler);

	mkfifo("queue", 0666);
	int fd = open("queue", O_RDONLY);
	while (running) {
		char buf[512];
		read(fd, buf, sizeof(buf));
		printf("got \"%s\"!\n", buf);

		evaluate_rules(&ctx, ruleset, num_rules);
	}

	close(fd);
	unlink("queue");
	*/
out:
	destroy_rules(ruleset, num_rules);
	return 0;
}
