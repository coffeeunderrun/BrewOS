STARTUP=BrewOSPkg/Loader/startup.nsh
LOADER=edk2/Build/BrewOS/DEBUG_GCC5/X64/Loader.efi
KERNEL=bin/kernel
IMAGE=brewos.img
ASSETS=assets

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

# Make drive image (40 MiB, FAT32)
echo
echo Building drive image...
dd if=/dev/zero of=$IMAGE bs=512 count=81920
mformat -i $IMAGE -F ::
mmd -i $IMAGE ::/EFI
mmd -i $IMAGE ::/EFI/BOOT
mcopy -i $IMAGE $LOADER ::/EFI/BOOT
mcopy -i $IMAGE $STARTUP ::
mcopy -i $IMAGE $KERNEL ::
mcopy -i $IMAGE $ASSETS ::
