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

static ssize_t bof_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t bof_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

struct file_operations bof_fops = {
    .read   = bof_read,
    .write  = bof_write,
};

static struct miscdevice bof_driver = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "bof",
    .fops = &bof_fops,
};

static ssize_t bof_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
    char arr[32] = { [0 ... 31] = 0 };
    char *ptr;
    unsigned char len;

    if(count > 32) {
        printk("I hate hackers!!");
        return -1;
    }

    len = (unsigned char)count;
    len -= 1;

    ptr = (char *)kzalloc(len, GFP_KERNEL);
    memcpy(ptr, arr, len);

    copy_to_user(buf, ptr, len);

    printk("arr : %x", arr[len]);  // prevent undefined behavior

    return 0;
}


static ssize_t bof_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    char arr[32] = { [0 ... 31] = 0 };
    char *ptr;
    unsigned char len;

    if(count > 32) {
        printk("I hate hackers!!");
        return -1;
    }

    len = (unsigned char)count;
    len -= 1;

    ptr = (char *)kmalloc(len, GFP_KERNEL);

    copy_from_user(ptr, buf, len);
    memcpy(arr, ptr, len);

    printk("arr : %x", arr[len]);  // prevent undefined behavior

    return 0;
}


static int bof_init(void) {
    int result;

    result = misc_register(&bof_driver);

    return 0;
}

static void bof_exit(void) {
    misc_deregister(&bof_driver);
}

module_init(bof_init);
module_exit(bof_exit);
