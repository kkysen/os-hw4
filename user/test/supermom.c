#include "supermom.h"

#include <sys/syscall.h>
#include <unistd.h>

int supermom(pid_t pid, uid_t *uid)
{
	return (int)syscall(__NR_supermom, pid, uid);
}
