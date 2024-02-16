obj-m += hellok.o 
 
PWD := $(CURDIR) 

all: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 

load:
	sudo insmod hellok.ko

unload:
	sudo rmmod hellok

show-jrnl:
	sudo journalctl --since "1 hour ago" | grep kernel

clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean