.PHONY: all
all: fibo.o main.o

fibo.o:
	gcc -c -I../include/ fibo.c -o fibo.o

main.o:
	gcc -c -I../include/ main.c -o main.o

.PHONY: clean
clean:
	rm -f fibo.o
	rm -f main.o
