#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>

static int __init ofd_init(void) {	// constructor
	printk(KERN_INFO "Kloenk: ofd registered");

	return 0;
}

static void __exit ofd_exit(void) {	// destructor
	printk(KERN_INFO "Alvida: ofd unregistered");
}

module_init(ofd_init);
module_exit(ofd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kloenk <kloenk@kloenk.de>");
MODULE_DESCRIPTION("Our first driver");
