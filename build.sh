TARGET=DEBUG
IMAGE=./Build/BrewOS.img

# Build UEFI boot loader
function buildLoader() {
    echo Building boot loader...
    pushd ./edk2
    . edksetup.sh && build -b ${TARGET} -a X64 -t GCC5 -p ../BootPkg/BootPkg.dsc -Y COMPILE_INFO -y ./Build/Boot/${TARGET}_GCC5/X64/Loader.rpt
    RESULT=$?
    popd
    return $RESULT
}

# Build kernel
function buildKernel() {
    echo
    echo Building kernel...
    pushd ./Build
    cmake -B . -S .. && make
    RESULT=$?
    popd
    return $RESULT
}

# Combine compile commands for linter
function compileCommands() {
    find . -type f -name compile_commands.json -not -path ./Build -print0 | xargs -0 jq -s '.' > ./Build/compile_commands.json
}

# Make drive image (40 MiB, FAT32)
function buildImage() {
    echo
    echo Building drive image...
    dd if=/dev/zero of=${IMAGE} bs=512 count=81920 \
        && mformat -i ${IMAGE} -F :: \
        && mmd -i ${IMAGE} ::/EFI \
        && mmd -i ${IMAGE} ::/EFI/BOOT \
        && mcopy -i ${IMAGE} ./edk2/Build/Boot/${TARGET}_GCC5/X64/Loader.efi ::/EFI/BOOT \
        && mcopy -i ${IMAGE} ./startup.nsh :: \
        && mcopy -i ${IMAGE} ./Build/bin/Kernel ::
}

mkdir -p ./Build && buildLoader && buildKernel && compileCommands && buildImage
