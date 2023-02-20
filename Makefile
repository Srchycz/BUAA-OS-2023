.PHONY: clean

out: calc case_all
	./calc < case_all > out

# Your code here.
calc:
	gcc -o calc calc.c

casegen:
	gcc -o casegen casegen.c

case_all: case_add case_sub case_mul case_div
	cat case_add >> case_all; \
	cat case_sub >> case_all; \
	cat case_mul >> case_all; \
	cat case_div >> case_all

case_add: casegen
	./casegen add 100 > case_add

case_sub: casegen
	./casegen sub 100 > case_sub

case_mul: casegen
	./casegen mul 100 > case_mul

case_div: casegen
	./casegen div 100 > case_div

clean:
	rm -f out calc casegen case_*
