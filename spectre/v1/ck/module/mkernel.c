
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/ioctl.h>

#include "common.h"


MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");

static int __init minit(void){
    printk(KERN_INFO "LEHR INIT\n");

    if(ioctl_device_init() < 0){
        printk(KERN_INFO "LEHR_KERNEL ioctl fail!\n");
        return -1;
    }
    

    return 0;
}

static void __exit mexit(void){
    ioctl_device_exit();
    printk(KERN_INFO "LEHR EXIT\n");
}

module_init(minit);
module_exit(mexit);