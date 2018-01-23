#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>

static ssize_t jiffies_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%lu\n", jiffies);
}

static const struct kobj_attribute jiffies_attribute = __ATTR_RO(jiffies);

static int __init example_init(void)
{
	int res;

	if ((res = sysfs_create_file(kernel_kobj, &jiffies_attribute.attr)) < 0) {
		printk(KERN_ERR "Failed to create sysfs entry (%d)\n", res);
		return res;
	}

	return 0;
}

static void __exit example_exit(void)
{
	sysfs_remove_file(kernel_kobj, &jiffies_attribute.attr);
}

module_init(example_init);
module_exit(example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lukasz Gemborowski <lukasz.gemborowski@gmail.com>");
MODULE_DESCRIPTION("Kernel module example");
