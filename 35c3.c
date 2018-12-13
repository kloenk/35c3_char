#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

static dev_t first;
static struct cdev c_dev;
static struct class *c1;
#define lenght 6
static char c[lenght] = "35c3\n";

static int my_open(struct inode *i, struct file *f) {
	printk(KERN_INFO "Kloenk: open()\n");
	return 0;
}

static int my_close(struct inode *i, struct file *f) {
	printk(KERN_INFO "Kloenk: close()\n");
	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
	//if (*off == 0) {
		if (copy_to_user(buf, &c, lenght) != 0)
			return -EFAULT;
		else {
			(*off)++;
			return 3;
		}
	//} else {
	//	return 0;
	//}
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
	printk(KERN_INFO "Kloenk: write()\n");
	if (copy_from_user(&c, buf + len - lenght, lenght) != 0)
		return -EFAULT;
	else 
		return len;
}

static struct file_operations pugs_fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write
};

static int __init ofd_init(void) {	// constructor
	int ret;
	struct device *dev_ret;

	printk(KERN_INFO "Kloenk: registered");
	if ((ret = alloc_chrdev_region(&first, 0, 1, "Kloenk")) < 0) {
		return ret;
	}

	if (IS_ERR(c1 = class_create(THIS_MODULE, "chardrv"))) {
		unregister_chrdev_region(first, 1);
		return PTR_ERR(c1);
	}

	if (IS_ERR(dev_ret = device_create(c1, NULL, first, NULL, "35c3"))) {
		class_destroy(c1);
		unregister_chrdev_region(first, 1);
		return PTR_ERR(dev_ret);
	}

	cdev_init(&c_dev, &pugs_fops);
	if ((ret = cdev_add(&c_dev, first, 1)) < 0) {
		device_destroy(c1, first);
		class_destroy(c1);
		unregister_chrdev_region(first, 1);
		return ret;
	}
	return 0;
}

static void __exit ofd_exit(void) {	// destructor
	cdev_del(&c_dev);
	device_destroy(c1, first);
	class_destroy(c1);
	unregister_chrdev_region(first, 1);
	printk(KERN_INFO "Kloenk: unregisterd");
}

module_init(ofd_init);
module_exit(ofd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kloenk <kloenk@kloenk.de>");
MODULE_DESCRIPTION("35c3 char dev");
