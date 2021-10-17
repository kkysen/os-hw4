#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>

static long supermom(const pid_t pid, uid_t *const uid)
{
	const struct task_struct *parent;
	kuid_t euid;
	bool is_root;

	parent = current->real_parent;

	if (parent->pid != pid) {
		pr_info("%s\n", "Not Yo Mama");
		return -EINVAL;
	}

	euid = current_euid();
	is_root = uid_eq(euid, GLOBAL_ROOT_UID);

	if (uid) {
		// use munged version so it never fails
		const uid_t user_euid =
			from_kuid_munged(current_user_ns(), euid);
		if (copy_to_user(uid, &user_euid, sizeof(*uid)) != 0)
			return -EFAULT;
	}

	return is_root ? 0 : -EACCES;
}

SYSCALL_DEFINE2(supermom, pid_t, pid, uid_t *, uid)
{
	return supermom(pid, uid);
}
