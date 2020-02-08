KERNELDIR ?=/home/david/linux-2.6.32.2
all: beep_test 

beep_test : beep_test.c
	#arm-linux-gcc -I$(KERNELDIR) -s -Wl,-warn-common --static -o $@ $^
	arm-linux-gcc -I$(KERNELDIR) -o $@ $^

clean :
	rm beep_test
	
	
	
#不知道为什么，linux路径不对，make不会抱错   2018/12/10
