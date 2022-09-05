STARTUP=BrewOSPkg/Loader/startup.nsh
BOOT=edk2/Build/BrewOS/DEBUG_GCC5/X64/Loader.efi
LOADER=bin/loader.elf
KERNEL=bin/kernel.elf
IMAGE=brewos.img

# Build UEFI boot loader
echo Building boot loader...
cd edk2
. edksetup.sh
build -a X64 -t GCC5 -p ../BrewOSPkg/BrewOSPkg.dsc
cd ..

# Build kernel
echo
echo Building kernel...
cmake .
make

# Make drive image
echo
echo Building drive image...
dd if=/dev/zero of=$IMAGE bs=512 count=20480
mformat -i $IMAGE -f 1440 ::
mmd -i $IMAGE ::/EFI
mmd -i $IMAGE ::/EFI/BOOT
mcopy -i $IMAGE $BOOT ::/EFI/BOOT
mcopy -i $IMAGE $LOADER ::
mcopy -i $IMAGE $KERNEL ::
mcopy -i $IMAGE $STARTUP ::
