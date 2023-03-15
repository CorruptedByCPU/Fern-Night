# Info

Fern-Night is now fully compatible with ![Cyjon](https://github.com/CorruptedByCPU/Cyjon/). Any modification to either repository will be reflected in the other.

# Fern-Night (workname)

A simple, clean, multi-tasking operating system written in C language for 64-bit processors from the AMD64 family.

![Cyjon 0.2099](https://blackdev.org/shot/2110.png?)

### Software:

  - Qemu 5.0.0 (or newer)

## Download

   https://blackdev.org/files/fern-night.iso

### Run

    qemu-system-x86_64 --enable-kvm -cdrom fern-night.iso -m 128 -cpu max -smp 2 -rtc base=localtime
