ifneq ($(KERNELRELEASE),)
mymodule-objs := myblock.o
obj-m := myblock.o
else
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	rm -rf *.ko *.o *.mod.c *.symvers *.order
endif
