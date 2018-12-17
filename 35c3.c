#include "35c3.h"

static dev_t first;
static struct cdev c_dev;
static struct cdev c_dev2;
static struct class *c1;
static char c[length] = "35c3·";

static int my_open(struct inode *i, struct file *f) {
	printk(KERN_DEBUG "35c3: open()\n");
	return 0;
}

static int my_close(struct inode *i, struct file *f) {
	printk(KERN_DEBUG "35c3: close()\n");
	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
	if (copy_to_user(buf, &c, length) != 0)
		return -EFAULT;
	else {
		(*off)++;
		return length;
	}
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
	printk(KERN_INFO "35c3: write()\n");
	if (copy_from_user(&c, buf + len - length, length) != 0)
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

static void daysTo(char *buf, time_t ns) {
	int years, days, hours, minuts;
	bool used = false;

	years = ns / yearNS;
	if (years >= 1) {
		ns = ns - (years * yearNS);
		sprintf(buf, "%d years, ", years); // create better , 
		used = true;
	} else { years = 0; }

	days = ns / dayNS;
	if (days >= 2) {
		ns = ns - (days * dayNS);
		if (used) {
			sprintf(buf, "%s%d days, ", buf, days);
		} else {
			sprintf(buf, "%d days, ", days);
		}
		used = true;
	} else { days = 0; }

	hours = ns / hourNS;
	if (hours >= 2) {
		ns = ns - (hours * hourNS);
		if (used) {
			sprintf(buf, "%s%d hours, ", buf, hours);
		} else {
			sprintf(buf, "%d hours, ", hours);
		}
		used = true;
	} else { hours = 0; }

	minuts = ns / minutNS;
	if (minuts >= 5) {
		ns = ns - (minuts * minutNS);
		if (used) {
			sprintf(buf, "%s%d minuts, ", buf, minuts);
		} else {
			sprintf(buf, "%d minuts, ", minuts);
		}
		used = true;
	} else { minuts = 0; }

	if (used) {
		sprintf(buf, "%s and %ld seconds\n", buf, ns);
	} else {
		sprintf(buf, "%ld seconds to go\n", ns);
	}

	// create char array
}

static ssize_t read_days_to(struct file *f, char __user *buf, size_t len, loff_t *off) {

	if (*off == 0) {
		struct timespec ts;
		time_t deltaTime;

		getnstimeofday(&ts);

		if (ts.tv_sec < congressNS) {
			deltaTime = congressNS - ts.tv_sec;
		} else {
			deltaTime = ts.tv_sec - congressEndNS;
		}

		char buffer[200];
		daysTo(buffer, deltaTime);
		printk(KERN_INFO "35C3Count: time: %s", buffer);


		if (copy_to_user(buf, &buffer, strlen(buffer)) != 0)
			return -EFAULT;
		else {
			(*off)++;
			return strlen(buffer);
		}
		(*off)++;
	} else {
		return 0;
	}
}

static struct file_operations count_fops = {
	.owner   = THIS_MODULE,
	.open    = my_open,
	.release = my_close,
	.read    = read_days_to
};

static int __init c3_init(void) {	// constructor
	int ret;
	struct device *dev_ret;

	printk(KERN_INFO "35c3: registered\n");
	if ((ret = alloc_chrdev_region(&first, 0, charDevCount, "35c3")) < 0) {
		return ret;
	}

	if (IS_ERR(c1 = class_create(THIS_MODULE, "chardrv"))) {
		unregister_chrdev_region(first, charDevCount);
		return PTR_ERR(c1);
	}

	if (IS_ERR(dev_ret = device_create(c1, NULL, first, NULL, "35c3"))) {
		class_destroy(c1);
		unregister_chrdev_region(first, charDevCount);
		return PTR_ERR(dev_ret);
	}

	if (IS_ERR(dev_ret = device_create(c1, NULL, first + 1, NULL, "35c3Count"))) {
		device_destroy(c1, first);
		class_destroy(c1);
		unregister_chrdev_region(first, charDevCount);
		return PTR_ERR(dev_ret);
	}

	printk(KERN_INFO "DEVICES CREATED\n");

	cdev_init(&c_dev, &pugs_fops);
	if ((ret = cdev_add(&c_dev, first, 1)) < 0) {
		device_destroy(c1, first);
		device_destroy(c1, first + 1);
		class_destroy(c1);
		unregister_chrdev_region(first, charDevCount);
		return ret;
	}

	cdev_init(&c_dev2, &count_fops);
	if ((ret = cdev_add(&c_dev2, first +1 , 1)) < 0) {
		cdev_del(&c_dev);
		device_destroy(c1, first);
		device_destroy(c1, first + 1);
		class_destroy(c1);
		unregister_chrdev_region(first, charDevCount);
		return ret;
	}

	return 0;
}

static void __exit c3_exit(void) {	// destructor
	cdev_del(&c_dev);
	cdev_del(&c_dev2);
	device_destroy(c1, first);
	device_destroy(c1, first + 1);
	class_destroy(c1);
	unregister_chrdev_region(first, charDevCount);
	printk(KERN_INFO "35c3: unregisterd\n");
}

module_init(c3_init);
module_exit(c3_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kloenk <kloenk@kloenk.de>");
MODULE_DESCRIPTION("35c3 char dev");
