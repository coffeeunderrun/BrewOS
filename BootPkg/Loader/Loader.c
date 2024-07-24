#include <Common.h>
#include <Uefi.h>
#include <Library/ElfLib.h>
#include <Library/GraphicsOutputLib.h>
#include <Library/MemoryMapLib.h>
#include <Library/UefiLib.h>

typedef struct KernelData {
    VOID* start;
    UINTN pageCount;
    MemoryMap* memoryMap;
    GraphicsOutput* graphicsOutput;
} KernelData;

#ifndef MDEPKG_NDEBUG
static EFI_STATUS Pause(EFI_SYSTEM_TABLE* systemTable) {
    EFI_STATUS status;

    UINTN keyEvent = 0;
    status = systemTable->BootServices->WaitForEvent(1, &systemTable->ConIn->WaitForKey, &keyEvent);
    if (EFI_ERROR(status)) {
        return status;
    }

    return systemTable->ConIn->Reset(systemTable->ConIn, FALSE);
}
#endif

EFI_STATUS EFIAPI UefiEntry(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable) {
    EFI_STATUS status;

#ifndef MDEPKG_NDEBUG
    Print(L"Launch LOADER debug and press any key to continue...\n");
    Pause(systemTable);
#endif

    // Load kernel image into memory wherever UEFI sees fit
    ElfImage kernelImage;
    status = LoadElfImage(&kernelImage, imageHandle, L"Kernel");
    if (EFI_ERROR(status)) {
        Print(L"Unable to load KERNEL.\n");
        return status;
    }

#ifndef MDEPKG_NDEBUG
    Print(L"Launch KERNEL debug and press any key to continue...\n");
    Pause(systemTable);
#endif

    // Get graphics frame buffer
    GraphicsOutput graphicsOutput;
    status = GetGraphicsOutput(&graphicsOutput, imageHandle, 640, 480);
    RETURN_IF_ERROR_STATUS(status);

    // Get physical memory map
    MemoryMap memoryMap;
    status = GetMemoryMap(&memoryMap);
    RETURN_IF_ERROR_STATUS(status);

    // Prepare to hand control over to kernel
    status = systemTable->BootServices->ExitBootServices(imageHandle, memoryMap.mapKey);
    RETURN_IF_ERROR_STATUS(status);

    // Prepare data to pass to kernel
    KernelData kernelData = {
        kernelImage.start,
        kernelImage.pageCount,
        &memoryMap,
        &graphicsOutput,
    };

    // Jump to kernel
    if (kernelImage.Execute != NULL) {
        kernelImage.Execute(&kernelData);
    }

    // Kernel does not return so this point should never be reached
    systemTable->RuntimeServices->ResetSystem(EfiResetShutdown, 0, 0, NULL);

    return status;
}
