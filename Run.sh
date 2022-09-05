IMAGE=brewos.img
OVMF=/usr/share/ovmf/OVMF.fd

qemu-system-x86_64 -s -S -m 256M -cpu qemu64 -net none \
    -drive format=raw,file=$IMAGE \
    -drive if=pflash,format=raw,unit=0,file=$OVMF,readonly=on
