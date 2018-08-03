/*
*  TQ210开发板LED驱动测试，需手动创建设备文件
*/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/sched.h>

#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>

#include <linux/slab.h>
#define DEVICE_NAME "leds"

static int led_gpios[] = {
	S5PV210_GPC0(3),
	S5PV210_GPC0(4),
};

#define LED_NUM ARRAY_SIZE(led_gpios)


#define OK 			(0)
#define ERROR 		(-1)

int reg_major = 232;;
int reg_minor = 0;
dev_t dev_num;
unsigned int sub_dev_num = 1;
struct cdev *chr_dev;
struct file_operations *file_op;
char kbuf[4] = {0};

static int leds_open(struct inode *p, struct file *f)
{

	printk(KERN_EMERG "leds_open\r\n");

	return 0;
}

static int leds_close(struct inode *inode, struct file *f)
{
	printk(KERN_EMERG "leds_close\r\n");
	return 0;
}
static ssize_t leds_write(struct file *f, const char __user *u, size_t s, loff_t *l)
{
	int n = 0;
	int len = 4;
	int i;
	printk(KERN_EMERG "leds_write\r\n");

	n = copy_from_user(kbuf, u, len);
	if(n != 0) return -ENOMEM;

	for(i=0; i<LED_NUM; i++) {
		//gpio_set_value(led_gpios[i], kbuf[i]);	//该API不行
		gpio_direction_output(led_gpios[i], kbuf[i]);		
	}

	return len;
}
static ssize_t leds_read(struct file *f, char __user *u, size_t s, loff_t *l)
{
	printk(KERN_EMERG "leds_read\r\n");
	return 0;
}
static long leds_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	printk(KERN_EMERG "leds_ioctl\r\n");
	switch(cmd)
	{
		case 0:
		case 1:
		
			if(arg > LED_NUM) {
				return -EINVAL;
			}
			//gpio_set_value(led_gpios[arg], cmd);	//该API不行
			gpio_direction_output(led_gpios[arg], cmd);		
	
		default:
			return -EINVAL;	
		break;
	}

	return 0;
}

// initialize char device
static int __init char_dev_init(void)
{
	int err;
	int i;

	dev_num = MKDEV(reg_major, reg_minor);
	printk(KERN_EMERG "dev_num is %d\r\n", dev_num);

	if(OK == register_chrdev_region(dev_num, sub_dev_num, DEVICE_NAME)) {
		printk(KERN_EMERG "register_chrdev_region ok.\r\n");
	} else {
		printk(KERN_EMERG "register_chrdev_region error.\r\n");
		return ERROR;
	}

	chr_dev = kzalloc(sizeof(struct cdev), GFP_KERNEL);
	file_op = kzalloc(sizeof(struct file_operations), GFP_KERNEL);

	file_op->open = leds_open;
	file_op->release = leds_close;
	file_op->read = leds_read;
	file_op->write = leds_write;
	file_op->unlocked_ioctl = leds_ioctl;
	file_op->owner = THIS_MODULE;
	cdev_init(chr_dev, file_op);	//初始化字符设备结构体
	cdev_add(chr_dev, dev_num, 3);	//添加字符设备内核

	err = gpio_request(led_gpios[0], "GPC0_3");	//请求分配GPIO资源
	if(err) {
		printk(KERN_EMERG "failed to request GPC0_3 for LED1 pin\n");

		return err;
	}
	err = gpio_request(led_gpios[1], "GPC0_4");	//请求分配GPIO资源
	if(err) {
		printk(KERN_EMERG "failed to request GPC0_4 for LED2 pin\n");

		return err;
	}	
	
	for(i=0; i<LED_NUM; i++) {
		gpio_direction_output(led_gpios[i],0);				//GPIO设置成输出，并设置成低电平
	}
	
	return 0;
}


void __exit char_dev_exit(void)
{
	int i;
	
	for(i=0; i<LED_NUM; i++) {
		gpio_free(led_gpios[i]);	//释放GPIO系统资源
	}
	
	cdev_del(chr_dev);
	unregister_chrdev_region(dev_num, sub_dev_num);

	return;
}

module_init(char_dev_init);
module_exit(char_dev_exit);
MODULE_LICENSE("GPL");

