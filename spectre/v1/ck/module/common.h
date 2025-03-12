#ifndef __COMMON_H
#define __COMMON_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/device.h>
/* 参考资料: https://www.cnblogs.com/downey-blog/p/10502235.html */

#define TEST_IOCTL_ZERO 0
#define TEST_IOCTL_FIRST 1
#define TEST_IOCTL_SECOND 2

#define DATA_DEVICE_NAME "mdata"
#define IOCTL_DEVICE_NAME "mioctl"
#define IOCTL_DEVICE_CLASS_NAME "mioctl_class"

#define MEM_SIZE 256*4096

/*注意在需要被多文件共享的函数声明的时候不能使用static关键字
*因为static关键字的最重要的作用就是隐藏，加了static关键字的变量和函数都不能全局可见
*/
// static int ioctl_device_init(void);
 int ioctl_device_init(void);
 void ioctl_device_exit(void);
 long test_ioctl(struct file * , unsigned int , unsigned long );

#endif