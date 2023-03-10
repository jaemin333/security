#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define AAR 0x11
#define AAW 0x22
#define TASK_STRUCT 0x33

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("V4bel");

struct elements {
    void *to;
    void *from;
};

static long test_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

struct file_operations test_fops = {
    .unlocked_ioctl = test_ioctl,
};

static struct miscdevice test_driver = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "test",
    .fops = &test_fops,
};

static long test_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
    struct elements *elements;
    void *ptr;

    elements = (struct elements *)arg;
    
    switch(cmd) {
        case AAR:
            if(copy_to_user(elements->to, elements->from, sizeof(void *)))
                return -EFAULT;

            break;
        case AAW:
            if(copy_from_user(elements->to, elements->from, sizeof(void *)))
                return -EFAULT;

            break;
        case TASK_STRUCT:
            ptr = get_current();

            if(copy_to_user(elements->to, &ptr, sizeof(void *)))
                return -EFAULT;

            break;
        default:
            break;
    }

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
