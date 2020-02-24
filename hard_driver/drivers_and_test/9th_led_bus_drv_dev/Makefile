KERN_DIR = /home/book/github/weidongshan/linux-2.6.22.6
all:
	make -C $(KERN_DIR) M=`pwd` modules 
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order
	
obj-m	+= led_drv.o
obj-m	+= led_dev.o
