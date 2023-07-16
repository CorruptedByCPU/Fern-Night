qemu-system-x86_64	\
	-s -S		\
	--enable-kvm	\
	-cpu max	\
	-smp 2		\
	-m 128		\
	-cdrom build/fern-night.iso	\
	-netdev user,id=ethx -device e1000,netdev=ethx	\
	-rtc base=localtime 				\
	-serial file:serial.log &
sleep 1
./r2.sh
#gdb build/kernel

