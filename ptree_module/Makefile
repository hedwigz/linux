obj-m += ptree.o

all:
	make -C .. M=$(PWD) modules
clean:
	make -C .. M=$(PWD) clean

test:
	sudo dmesg -C
	sudo insmod ptree.ko
	sudo rmmod ptree.ko
