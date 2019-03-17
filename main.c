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

int pipefd[2];
struct engine *engine;

void sighandler(int num) {
	switch (num) {
		case SIGINT:
			DEBUG("shutting down\n");
			write(pipefd[1], NULL, 0);
			running = 0;
			break;
		case SIGHUP:
			// TODO: keep fallback copy
			DEBUG("reload config\n");
			if (engine_load_rules(engine) != 0) {
				DEBUG("Unable to load rules, abort\n");
				exit(1);
			}
			break;
		default:
			break;
	}
}

int main(int argc, char **argv) {
	if (pipe(pipefd) == -1) {
		goto out;
	}

	char *rules_config = argc > 1 ? argv[1] : "rules.txt";
	engine = create_engine(rules_config);
	if (engine == NULL) {
		DEBUG("unable to create engine from rules %s\n", rules_config);
		goto destroy;
	}

	signal(SIGINT, sighandler);
	signal(SIGHUP, sighandler);

	char *fifo = "queue";
	mkfifo(fifo, 0666);
	int fd = open(fifo, O_RDWR);

	while (running) {
		fd_set readers;
		struct timeval tv;
		FD_ZERO(&readers);
		FD_SET(fd, &readers);
		FD_SET(pipefd[0], &readers);
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		if (select(fd+1, &readers, NULL, NULL, &tv)) {
			if (/* shutdown pipe */ FD_ISSET(pipefd[0], &readers)) {
			} else if (/* fifo */ FD_ISSET(fd, &readers)) {
				char buf[512];
				memset(buf, 0, sizeof(buf));
				int read_n = read(fd, buf, sizeof(buf));
				if (read_n > 0) {
					puts(buf);
					struct context ctx = {
						.url = buf,
						.status = CTX_NONE,
					};
					engine_evaluate(engine, &ctx);
				}
		       }
		}
	}

	close(fd);
	unlink(fifo);
destroy:
	destroy_engine(engine);
out:
	return 0;
}
