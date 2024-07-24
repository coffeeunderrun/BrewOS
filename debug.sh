IMAGE=./Build/BrewOS.img

qemu-system-x86_64 \
    -cpu qemu64 -m 256M -net none \
    -gdb tcp::1234 -debugcon file:./Build/debug.log -global isa-debugcon.iobase=0x402 \
    -chardev stdio,id=char0,logfile=./Build/serial.log,signal=off -serial chardev:char0 \
    -drive format=raw,file=${IMAGE} \
    -drive if=pflash,format=raw,unit=0,file=./edk2/Build/OvmfX64/DEBUG_GCC5/FV/OVMF_CODE.fd,readonly=on \
    -drive if=pflash,format=raw,unit=1,file=./edk2/Build/OvmfX64/DEBUG_GCC5/FV/OVMF_VARS.fd
