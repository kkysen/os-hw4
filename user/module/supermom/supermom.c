#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/cred.h>

#define MODULE_NAME "Supermom"

typedef long (*supermom_handler_t)(pid_t pid, uid_t *uid);

extern void set_supermom_handler(supermom_handler_t handler);

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

	euid = current_cred()->euid;
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

int simple_init(void)
{
	pr_info("Loading %s\n", MODULE_NAME);
	set_supermom_handler(supermom);
	return 0;
}

void simple_exit(void)
{
	pr_info("Removing %s\n", MODULE_NAME);
	set_supermom_handler(NULL);
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(MODULE_NAME);
MODULE_AUTHOR("ks3343");
