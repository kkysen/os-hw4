#include <linux/module.h>
#include <linux/printk.h>

#define MODULE_NAME "Supermom"

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
MODULE_AUTHOR("cs4118");
