ifeq ($(KERNELRELEASE),)

KERNELDIR ?=/home/student/linux-2.6.32.2

PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order Module*

.PHONY: modules modules_install clean

else
    obj-m := hello.o
endif
