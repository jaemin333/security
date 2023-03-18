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

static ssize_t df_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

struct file_operations df_fops = {
    .write  = df_write,
};

static struct miscdevice df_driver = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "df",
    .fops = &df_fops,
};

char dummy[] = {"I have to put something, but I have nothing to say. This is dummy data"};

static ssize_t df_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    char arr[32] = { [0 ... 31] = 0 };
    char *ptr;
    int len = 0;
    int i = 0;
    
    if(*(int *)count > 32 || *(int *)count < 0) {
        printk("noob!!");

        return -1;
    }

    /* garbage */
    for(i=0; i<sizeof(dummy); i++)
        printk("%d", dummy[i]);

    len = *(int *)count;
    ptr = (char *)kmalloc(len, GFP_KERNEL);

    copy_from_user(ptr, buf, len);
    memcpy(arr, ptr, len);

    printk("arr : %x", arr[len-1]);  // prevent undefined behavior

    return 0;
}


static int df_init(void) {
    int result;

    result = misc_register(&df_driver);

    return 0;
}

static void df_exit(void) {
    misc_deregister(&df_driver);
}

module_init(df_init);
module_exit(df_exit);
