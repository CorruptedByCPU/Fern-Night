# Fern-Night
A slightly different approach than Lotus-Effect...

In this compilation, I'm rewriting daemons as regular programs (ring 3). In addition, all logical processors have been unlocked and are involved in the processing of jobs on the queue.

## Download
https://blackdev.org/files/fern-night.iso

### Run
    qemu-system-x86_64 --enable-kvm -cdrom lotus-effect.iso -m 64 -cpu max -smp 2 -rtc base=localtime
