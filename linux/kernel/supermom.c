#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE2(supermom, pid_t, pid, uid_t *, uid)
{
	return -ENOSYS;
}
