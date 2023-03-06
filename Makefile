.PHONY: all
all: test

test: test.c
	gcc -o test test.c

.PHONY: run
run: test
	./test

.PHONY: clean
clean: 
	rm -f test
