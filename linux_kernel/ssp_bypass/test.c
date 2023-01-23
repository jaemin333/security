#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/string.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("V4bel");

static ssize_t test_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t test_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

struct file_operations test_fops = {
    .read   = test_read,
    .write  = test_write,
};

static struct miscdevice test_driver = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "test",
    .fops = &test_fops,
};

static ssize_t test_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
    char arr[8] = { [0 ... 7] = 0 };
    char *ptr;

    ptr = (char *)kzalloc(count, GFP_KERNEL);
    memcpy(ptr, arr, count);

    copy_to_user(buf, ptr, count);

    return 0;
}


static ssize_t test_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    char arr[8] = { [0 ... 7] = 0 };
    char *ptr;

    ptr = (char *)kmalloc(count, GFP_KERNEL);

    copy_from_user(ptr, buf, count);
    memcpy(arr, ptr, count);
    
    printk("arr : %x", arr[count-1]);  // prevent undefined behavior

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
