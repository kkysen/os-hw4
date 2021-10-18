#include <linux/module.h>

#define MODULE_NAME "Supermom"

typedef long (*supermom_handler_t)(pid_t pid, uid_t *uid);

extern void set_supermom_handler(supermom_handler_t handler);

int simple_init(void)
{
	pr_info("Loading %s\n", MODULE_NAME);
	return 0;
}

void simple_exit(void)
{
	pr_info("Removing %s\n", MODULE_NAME);
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(MODULE_NAME);
MODULE_AUTHOR("ks3343");
