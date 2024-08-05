#!/bin/bash
#===============================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#===============================================================================

# for clear view
clear

git submodule update --init

rm -rf build && mkdir -p build
rm -rf system && mkdir -p system

OPT="${1}"
if [ -z "${OPT}" ]; then OPT="fast"; fi

C="clang"
LD="ld"
ASM="nasm"
ISO="xorriso"
CFLAGS="${CFLAGS} -std=c18 -O${OPT} -march=x86-64 -nostdlib -fomit-frame-pointer -fno-builtin -fno-stack-protector -m64 -mno-red-zone -fno-asynchronous-unwind-tables"
KERNEL_MODE="-mno-mmx -mno-sse -mno-sse2 -mno-3dnow"
LDFLAGS="-nostdlib -static -no-dynamic-linker"

# check environment software, required!
ENV=true
echo -n "Fern-Night environment: "
	type -a ${C} &> /dev/null || (echo -en "\e[38;5;196m${C}\e[0m" && ENV=false)
	type -a ${LD} &> /dev/null || (echo -en "\e[38;5;196m${LD}\e[0m" && ENV=false)
	type -a ${ASM} &> /dev/null || (echo -en "\e[38;5;196m${ASM}\e[0m" && ENV=false)
	type -a ${ISO} &> /dev/null || (echo -en "\e[38;5;196m${ISO}\e[0m" && ENV=false)
if [ "${ENV}" = false ]; then echo -e "\n[please install missing software]"; exit 1; else echo -e "\e[38;5;2m\xE2\x9C\x94\e[0m"; fi

# optional
type -a qemu-system-x86_64 &> /dev/null || echo -e "Optional \e[38;5;11mqemu\e[0m not installed. ISO file will be generated regardless of that."

${ASM} -f elf64 kernel/hpet.asm -o build/hpet.o
${ASM} -f elf64 kernel/service.asm -o build/service.o
${ASM} -f elf64 kernel/task.asm -o build/task.o
${ASM} -f elf64 kernel/init/idt.asm -o build/idt.o
${ASM} -f elf64 kernel/init/gdt.asm -o build/gdt.o
${ASM} -f elf64 kernel/driver/ps2.asm -o build/ps2.o

${C} -c kernel/init.c -o build/kernel.o ${CFLAGS} ${KERNEL_MODE} || exit 1;
${LD}  build/hpet.o build/service.o build/task.o build/gdt.o build/idt.o build/ps2.o build/kernel.o -o build/kernel -T linker.kernel ${LDFLAGS}
strip -R .comment -s build/kernel

# information
kernel_size=`ls -lh build/kernel | cut -d ' ' -f 5`
echo -e "${green}\xE2\x9C\x94${default}|Kernel|${kernel_size}" | awk -F "|" '{printf "%s  %-30s %s\n", $1, $2, $3 }'

lib=""

for lib_independent in color elf font integer math rgl string; do
	${C} -c -fpic library/shared/${lib_independent}.c -o build/${lib_independent}.o ${CFLAGS} || exit 1
	${C} -shared build/${lib_independent}.o -o system/lib${lib_independent}.so ${CFLAGS} -Wl,--as-needed,-Tlinker.library -Lsystem/ ${lib} || exit 1
	strip -R .comment -s system/lib${lib_independent}.so > /dev/null 2>&1
	lib="${lib} -l${lib_independent} "
done

for lib_dependent in terminal json sys random image interface; do
	name=${shared::$(expr ${#shared} - 2)}
	${C} -c -fpic library/shared/${lib_dependent}.c -o build/${lib_dependent}.o ${CFLAGS} || exit 1
	${C} -shared build/${lib_dependent}.o -o system/lib${lib_dependent}.so ${CFLAGS} -Wl,--as-needed,-Tlinker.library -Lsystem/ ${lib} || exit 1
	strip -R .comment -s system/lib${lib_dependent}.so > /dev/null 2>&1
	lib="${lib} -l${lib_dependent} "
done

if [ `ls daemon | wc -l` -ne 0 ]; then
	for daemon in `(cd daemon && ls *.c)`; do
		name=${daemon::$(expr ${#daemon} - 2)}
		${C} -c -fpic daemon/${name}.c -o build/${name}.o ${CFLAGS} ${KERNEL_MODE} || exit 1
		${LD} build/${name}.o -o system/${name} -T linker.daemon ${LDFLAGS}
		strip -R .comment -s system/${name} > /dev/null 2>&1
	done
fi

if [ `ls software | wc -l` -ne 0 ]; then
	for software in `(cd software && ls *.c)`; do
		name=${software::$(expr ${#software} - 2)}
		${C} -c software/${name}.c -o build/${name}.o ${CFLAGS} || exit 1
		${LD} --as-needed -L./system build/${name}.o -o system/${name} ${lib} -T linker.software ${LDFLAGS}
		strip -R .comment -s system/${name} > /dev/null 2>&1
	done
fi

cp gfx/* system
cp home/* system

${C} vfs.c -o vfs
./vfs system

rm -rf iso
mkdir iso

cp limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso/
cp build/{kernel,system.vfs} iso

xorriso -as mkisofs -b limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label iso -o build/fern-night.iso > /dev/null 2>&1

limine/limine bios-install build/fern-night.iso > /dev/null 2>&1

rm -rf iso build/*.o