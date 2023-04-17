#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);

extern void handle_ov(void);

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
    [12] = handle_ov,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}

void do_ov(struct Trapframe *tf) {
	unsigned long epc = tf->cp0_epc;
	u_int *addr = (u_int *)KADDR(va2pa(cur_pgdir, epc));
	u_int instr = *addr;
	if ((instr>>26) == 0) {
		if( (instr & 2) == 0) {
			printk("add ov handled\n");
		}
		else {
			printk("sub ov handled\n");
		}
		instr |= 1;
	}
	else {
		printk("addi ov handled\n");
		int t = ((instr >> 16) & 0x1F);
		int s = ((instr >> 21) & 0x1F);
		int imm = (instr & 0x7FFF);
		if((instr >> 15) & 1) 
			imm = -imm;
		tf->regs[t] = tf->regs[s] / 2 + imm / 2;
		tf->cp0_epc += 4;
	}
	curenv->env_ov_cnt++;
}
