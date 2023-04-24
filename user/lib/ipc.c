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

u_int fa_id[100], tot = 0;
void ipc_broadcast(u_int val, void * srcva, u_int perm) {
	u_int me = syscall_getenvid();
	struct Env *curenv = &envs[ENVX(me)];
	fa_id[tot++] = curenv->env_id;
	int r;
	for (int i=0; i < NENV; ++i) {
		struct Env *e = &(envs[i]);
		if (e->env_status == ENV_FREE)
			continue;
		int flag=0;
		for(int j=0; j < tot; ++j) {
			if(fa_id[j] == e->env_parent_id) {
				flag=1;
				break;
			}
		}
		if (flag) {
			fa_id[tot++] = e->env_id;
			u_int whom = e->env_id;
			while ((r = syscall_ipc_try_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
			 syscall_yield();
         		}
			user_assert(r == 0);
		}
		
	}
}
