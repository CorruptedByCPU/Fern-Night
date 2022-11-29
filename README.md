# Fern-Night
Synchronizing Cyjon code up to Fern-Night, please be patient.

In this compilation:

    - daemons run inside the software environment (ring 3),
    - logical processors have been unlocked,
    - (in progess) no more static libraries (full support for dynamic linking).

## Download
https://blackdev.org/files/fern-night.iso

### Run
    qemu-system-x86_64 --enable-kvm -cdrom lotus-effect.iso -m 64 -cpu max -smp 2 -rtc base=localtime
