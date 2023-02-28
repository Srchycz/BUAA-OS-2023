.PHONY: all
all: fibo

fibo: 
	$(MAKE) -C code/
	gcc code/fibo.o code/main.o -o fibo	

.PHONY: clean
clean:
	$(MAKE) -C code/ clean
