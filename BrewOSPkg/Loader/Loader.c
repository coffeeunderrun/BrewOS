#include <Common.h>
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/ElfLib.h>
#include <Library/MemoryMapLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

typedef __attribute__((sysv_abi)) VOID *EntryPoint(ElfImage *, MemoryMap *);

EFI_STATUS EFIAPI UefiEntry(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable)
{
    EFI_STATUS status;

    ElfImage loaderImage;
    status = LoadElfImage(L"loader.elf", imageHandle, &loaderImage);
    if (EFI_ERROR(status))
    {
        Print(L"Unable to load LOADER.\n");
        return status;
    }

    ElfImage kernelImage;
    status = LoadElfImage(L"kernel.elf", imageHandle, &kernelImage);
    if (EFI_ERROR(status))
    {
        Print(L"Unable to load KERNEL.\n");
        return status;
    }

    MemoryMap memoryMap;
    status = GetMemoryMap(&memoryMap);
    RETURN_IF_ERROR_STATUS(status);

    status = gBS->ExitBootServices(imageHandle, memoryMap.mapKey);
    RETURN_IF_ERROR_STATUS(status);

    // Recalculate loader entry based on location in memory
    EntryPoint *_loader = (EntryPoint *)((UINTN)loaderImage.entry + (UINTN)loaderImage.top);

    // Do not bother recalculating kernel entry point as the loader will handle mapping to higher half
    _loader(&kernelImage, &memoryMap);

    return status;
}
