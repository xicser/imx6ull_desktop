KERNELDIR := /home/wangxi/Xcode/kernel/imx6ull_kernel

CURRENT_PATH := $(shell pwd)

obj-m := indicator_drv.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean