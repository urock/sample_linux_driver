CPU_ARCH := powerpc
PWD := $(shell pwd)

ifneq ($(KERNELRELEASE),)

obj-m   := rosta_drv.o 

else

all:
	make ARCH=$(CPU_ARCH) CROSS_COMPILE=$(CROSS) -C $(KERNEL_DIR) M=$(PWD) modules
	chmod 777 rosta_drv.ko
clean:
	rm -f .*.cmd rosta_drv.ko *.c~ *.h~ *.o Makefile~ 
	rm -rf .tmp_version
	rm -rf .tmp_versions
endif