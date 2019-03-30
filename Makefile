MAJOR = 0
MINOR = 2
PATCHLEVEL = 0

VERSION = $(MAJOR).$(MINOR).$(PATCHLEVEL)

all: main reddit.so logging.so

%.o: CFLAGS+=-g
%.o: %.c
	gcc -c $< $(CFLAGS)

%.so: LDFLAGS+=-shared
%.so: %.o
	gcc -o $@ $< $(LDFLAGS)

main: LDFLAGS+=-ldl -export-dynamic
main: main.o rules.o engine.o
	gcc -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o *.so main

sloc: $(shell ls *.c *.h)
	gcc -E -fpreprocessed -dD -P $^ | wc -l

sign:
	env GPG_TTY=$(shell tty) git tag -s v$(VERSION) HEAD

.PHONY: clean sloc sign
