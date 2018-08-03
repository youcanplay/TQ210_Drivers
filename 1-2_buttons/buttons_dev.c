#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <linux/interrupt.h>


#include <mach/map.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>

#include <linux/slab.h>

#define DEVICE_NAME "buttons"

struct button_desc {
	int gpio;
	int number;
	char *name;
};

static struct button_desc buttons[] = {
	{S5PV210_GPH0(0), 0, "up"},
	{S5PV210_GPH0(1), 1, "down"},
	{S5PV210_GPH0(2), 2, "left"},
	{S5PV210_GPH0(3), 3, "right"},
	{S5PV210_GPH0(4), 4, "enter"},
	{S5PV210_GPH0(5), 5, "back"},
	{S5PV210_GPH2(6), 6, "home"},
	{S5PV210_GPH2(7), 7, "power"},
};

static char *button_state[2] = {"on", "off"};

static int reg_major  = 233;	//主设备号
static int reg_minor = 0;		//次设备号
static dev_t buttons_dev_num;	//按键设备号
static unsigned int sub_dev_num  = 1;	//子设备数
static struct cdev *buttons_dev;
static struct file_operations *buttons_op;

// 按键中断服务程序 ISR
static irqreturn_t button_interrupt(int irq, void *dev_id)
{
	struct button_desc *bdata = (struct button_desc *)dev_id;

	//int down;
	unsigned tmp;

	tmp = gpio_get_value(bdata->gpio);
	printk(KERN_EMERG "KEY %s : %s.\r\n", bdata->name, button_state[tmp&0x1]);

	return IRQ_HANDLED;
}

static int buttons_open(struct inode *p, struct file *f)
{
	int irq;
	int i = 0;
	int err = 0;

	printk(KERN_EMERG "buttons open.\r\n");
	for(i=0; i<ARRAY_SIZE(buttons); i++) {
		if(!buttons[i].gpio) {
			continue;
		}

		irq = gpio_to_irq(buttons[i].gpio);		//设置GPIO为外部中断线
		err = request_irq(irq, button_interrupt, IRQ_TYPE_EDGE_BOTH,	
			buttons[i].name, (void *)&buttons[i]);	//请求分配GPIO中断
		if(err)
			break;
	}

	return 0;
}
int __init buttons_dev_init(void)
{
	int ret = -1;

	buttons_dev_num = MKDEV(reg_major, reg_minor);	// 获取设备号
	printk(KERN_EMERG "buttons device number is %d\r\n", buttons_dev_num);

	// 注册设备号到内核
	ret = register_chrdev_region(buttons_dev_num, sub_dev_num, DEVICE_NAME);
	if(ret == 0) {
		printk(KERN_EMERG "register buttons device ok\r\n");
	} else {
		printk(KERN_EMERG "register buttons device failed\r\n");
		return -1;
	}

	buttons_dev = kzalloc(sizeof(struct cdev), GFP_KERNEL);
	buttons_op = kzalloc(sizeof(struct file_operations), GFP_KERNEL);

	buttons_op->owner = THIS_MODULE;
	buttons_op->open = buttons_open;
	cdev_init(buttons_dev, buttons_op);	//初始化按键设备buttons_dev结构体
	cdev_add(buttons_dev, buttons_dev_num, 1);	//添加按键设备buttons_dev到内核中
	printk(KERN_EMERG "buttons driver initial done ...\r\n");

	return 0;
}

void __exit buttons_dev_exit(void)
{
	cdev_del(buttons_dev);
	unregister_chrdev_region(buttons_dev_num, sub_dev_num);
	return ;
}

module_init(buttons_dev_init);
module_exit(buttons_dev_exit);
MODULE_LICENSE("GPL");
