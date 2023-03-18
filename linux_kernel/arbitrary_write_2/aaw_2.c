#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define SET 0x11
#define CRED 0x22

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("V4bel");

static long aaw_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

struct file_operations aaw_fops = {
    .unlocked_ioctl = aaw_ioctl,
};

static struct miscdevice aaw_driver = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "aaw_2",
    .fops = &aaw_fops,
};

void (*victim)(void) = NULL;

static long aaw_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
    struct task_struct *task_struct;
    struct cred *cred;

    switch(cmd) {
        case SET:
            if(copy_from_user(&victim, (void *)arg, sizeof(void*)))
                return -EFAULT;

            break;
        case CRED:
            task_struct = get_current();
            cred = task_struct->cred;
            
            if(copy_to_user((void *)arg, &cred, sizeof(void *)))
                return -EFAULT;

            break;
        default:
            victim();

            break;
    }

    return 0;
}

static void print_noob(void) {
    printk("my name is noob!!");
}

static int aaw_init(void) {
    int result;

    result = misc_register(&aaw_driver);

    victim = print_noob;

    return 0;
}

static void aaw_exit(void) {
    misc_deregister(&aaw_driver);
}

module_init(aaw_init);
module_exit(aaw_exit);
