ifneq ($(KERNELRELEASE),)
obj-m := char_dev.o
else
PWD := $(shell pwd)
KDIR := /opt/EmbedSky/TQ210/Kernel_2.6.35.7_TQ210_for_Linux_v1.4
# KDIR := /lib/modules/`uname -r`/build
all:
	make -C $(KDIR) M=$(PWD)
clean:
	rm -rf *.o *.ko *.mod.c *.symvers *.c~ *~ *.cmd *.order
endif
