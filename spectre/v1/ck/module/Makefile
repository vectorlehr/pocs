

#mkernel-y 和 mkernel-objs体现的作用也是不同的
#如果把源文件直接作为目标的话，会发生链接失败的情况
# obj-m += mkernel.o
# mkernel-y := mdevice.o 
obj-m += mkernel_demo.o
mkernel_demo-y := mdevice.o mkernel.o
# KDIR := /usr/src/linux
KDIR := /lib/modules/$(shell uname -r)/build
PWD 	:= $(shell pwd)


all: 
	make -C $(KDIR) M=$(PWD) modules

clean:
	rm -rf client
	make -C $(KDIR) M=$(PWD) clean

user:
	gcc -o client client.c

run: all
	./client
	dmesg | grep LEHR


rem:
	sudo rmmod mkernel

ins:
	sudo insmod mkernel.ko