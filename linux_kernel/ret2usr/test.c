#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("V4bel");

static ssize_t test_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

struct file_operations test_fops = {
    .write  = test_write,
};

static struct miscdevice test_driver = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "test",
    .fops = &test_fops,
};

static ssize_t test_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    int (*fp_exec)(void) = 0; 

    copy_from_user(&fp_exec, buf, sizeof(fp_exec));

    fp_exec();

    return 0;
}


static int test_init(void) {
    int result;

    result = misc_register(&test_driver);

    return 0;
}

static void test_exit(void) {
    misc_deregister(&test_driver);
}

module_init(test_init);
module_exit(test_exit);
