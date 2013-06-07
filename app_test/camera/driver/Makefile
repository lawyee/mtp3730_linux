ifeq ($(KERNELRELEASE),)
KERNELDIR ?=/home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk

PWD :=$(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(RM) -rf ".o

.PHONY: default clean

else
	obj-m :=ov5640
endif
