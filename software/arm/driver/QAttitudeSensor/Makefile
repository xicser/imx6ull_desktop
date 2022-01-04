KERNELDIR := /home/wangxi/Xcode/kernel/imx6ull_kernel

CURRENT_PATH := $(shell pwd)

app := icm20608_app
obj-m := icm20608_drv.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean

app:
	arm-linux-gnueabihf-gcc -march=armv7-a -mfpu=neon -mfloat-abi=hard $(app).c -o $(app)
app_clean:
	rm -rf ./$(app)