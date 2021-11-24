obj-m += maps.o

all:
	make -C .. M=$(PWD) modules
clean:
	make -C .. M=$(PWD) clean

test:
	sudo dmesg -C
	sudo insmod maps.ko
	sudo rmmod maps.ko
