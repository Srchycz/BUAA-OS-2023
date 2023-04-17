#include <asm/asm.h>

LEAF(msyscall)
	// Just use 'syscall' instruction and return.
	syscall
	jr	ra
	/* Exercise 4.1: Your code here. */

END(msyscall)
