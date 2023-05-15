// User-level IPC library routines

#include <env.h>
#include <lib.h>
#include <mmu.h>

// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm) {
	int r;
	while ((r = syscall_ipc_try_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		syscall_yield();
	}
	user_assert(r == 0);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm) {
	int r = syscall_ipc_recv(dstva);
	if (r != 0) {
		user_panic("syscall_ipc_recv err: %d", r);
	}

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}
//exam
u_int get_time(u_int *us) {
	int opt = 1;
	int ret = 0;
	syscall_write_dev(&opt, 0x15000000, 4);
	syscall_read_dev(us, 0x15000000 + 0x0020, 4);
	syscall_read_dev(&ret, 0x15000000 + 0x0010, 4);
	return ret;
}
void usleep(u_int us) {
	u_int us1;
	u_int s1 = get_time(&us1);
	while (1) {
		u_int us2;
		u_int s2 = get_time(&us2);
		long long c = (s2 - s1) * 1000000LL + (long long)(us2) - (long long)(us1);
		if (c >= us) {
			return;
		}
		else {
			syscall_yield();
		}
	}
}


