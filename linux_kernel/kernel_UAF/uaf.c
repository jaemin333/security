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

static int uaf_open(struct inode *inode, struct file *file);
static int uaf_release(struct inode *inode, struct file *file);
static ssize_t uaf_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t uaf_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

struct file_operations uaf_fops = {
    .open    = uaf_open,
    .release = uaf_release,
    .read    = uaf_read,
    .write   = uaf_write,
};

static struct miscdevice uaf_driver = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "uaf",
    .fops = &uaf_fops,
};

char *ptr = 0;

static int uaf_open(struct inode *inode, struct file *file) {
    if((ptr = kmalloc(0x90, GFP_KERNEL)) == NULL) {
        printk("kmalloc error");
        return -EFAULT;
    }

    return 0;
}

static int uaf_release(struct inode *inode, struct file *file){
    if(ptr)
        kfree(ptr);

    return 0;
}

static ssize_t uaf_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
    if(count > 0x90) {
        printk("I hate hackers!!");
        return -EPERM;
    }

    copy_to_user(buf, ptr, count);
    
    return 0;
}


static ssize_t uaf_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    if(count > 0x90) {
        printk("I hate hackers!!");
        return -EPERM;
    }

    copy_from_user(ptr, buf, count);

    return 0;
}


static int uaf_init(void) {
    int result;

    result = misc_register(&uaf_driver);

    return 0;
}

static void uaf_exit(void) {
    misc_deregister(&uaf_driver);
}

module_init(uaf_init);
module_exit(uaf_exit);
