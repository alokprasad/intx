#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>

int temp = 10;

static ssize_t test_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", temp);
}

static ssize_t test_store(struct kobject *kobj, struct kobj_attribute *attr, char *buf , size_t len)
{
    sscanf(buf,"%d",&temp);
    return len;
}


static const struct kobj_attribute test_attribute = __ATTR_RO(test);
 
static int __init example_init(void)
{

	sysfs_create_file(kernel_kobj, &test_attribute.attr);

	return 0;
}

static void __exit example_exit(void)
{
	sysfs_remove_file(kernel_kobj, &test_attribute.attr);
}

module_init(example_init);
module_exit(example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lukasz Gemborowski <lukasz.gemborowski@gmail.com>");
MODULE_DESCRIPTION("Kernel module example");
