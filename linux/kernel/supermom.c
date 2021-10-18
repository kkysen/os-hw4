#include <linux/syscalls.h>

typedef long (*supermom_handler_t)(pid_t pid, uid_t *uid);

static supermom_handler_t supermom_handler;

void set_supermom_handler(supermom_handler_t handler)
{
	supermom_handler = handler;
}

EXPORT_SYMBOL(set_supermom_handler);

SYSCALL_DEFINE2(supermom, pid_t, pid, uid_t *, uid)
{
	return supermom_handler ? supermom_handler(pid, uid) : -ENOSYS;
}
