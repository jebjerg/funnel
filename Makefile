all: main reddit.so logging.so

%.o: CFLAGS+=-g
%.o: %.c
	gcc -c $< $(CFLAGS)

%.so: CFLAGS+=-shared
%.so: %.o
	gcc -o $@ $< $(CFLAGS)

main: CFLAGS+=-ldl
main: main.o rules.o engine.o
	gcc  -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o *.so main

sloc: $(shell ls *.c *.h)
	gcc -E -fpreprocessed -dD -P $^ | wc -l

.PHONY: clean sloc
