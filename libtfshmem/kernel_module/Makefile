#Change this path if your kernel sources are somewhere else
KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	rm -f built-in.a *.o *.ko *.cmd *.mod.c *.order *.symvers.cmd *.ko.cmd *.symvers .[!.]*

install:
	sudo cp tfshmem.ko /lib/modules/`uname -r`/kernel/
	sudo depmod
	sudo cp tfshmem.conf /etc/modules-load.d/

uninstall:
	sudo rm -rf /lib/modules/`uname -r`/kernel/tfshmem.ko
	sudo depmod
	sudo rm -rf /etc/modules-load.d/tfshmem.ko

