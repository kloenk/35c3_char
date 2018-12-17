

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/time.h>


#define length 6	// length of char buffer
#define charDevCount 2	// numbers of char devices

#define congressNS	1545904800	// 12/27/18 10:00:00 AM UTC
#define congressEndNS	1545904800	// 12/27/18 10:00:00 AM UTC

#define yearNS  31536000
#define dayNS	86400
#define hourNS	3600
#define minutNS 60
