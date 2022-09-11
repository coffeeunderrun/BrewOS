#include <Common.h>
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/ElfLib.h>
#include <Library/GraphicsOutputLib.h>
#include <Library/MemoryMapLib.h>
#include <Library/PsfFontLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

typedef __attribute__((sysv_abi)) VOID *EntryPoint(VOID *, UINTN, MemoryMap *, GraphicsOutput *, PsfImage *);

EFI_STATUS EFIAPI UefiEntry(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable)
{
    EFI_STATUS status;

    // Get GOP frame buffer
    GraphicsOutput graphicsOutput;
    status = GetGraphicsOutput(&graphicsOutput, imageHandle, 800, 600);
    RETURN_IF_ERROR_STATUS(status);

    // Load font image
    PsfImage font;
    status = LoadPsfImage(L"zap-light20.psf", imageHandle, &font);
    if (EFI_ERROR(status))
    {
        Print(L"Unable to load FONT.\n");
        return status;
    }

    // Load kernel image into memory wherever UEFI sees fit
    ElfImage kernel;
    status = LoadElfImage(L"kernel.elf", imageHandle, &kernel);
    if (EFI_ERROR(status))
    {
        Print(L"Unable to load KERNEL.\n");
        return status;
    }

    // Get physical memory map
    MemoryMap memoryMap;
    status = GetMemoryMap(&memoryMap);
    RETURN_IF_ERROR_STATUS(status);

    // Prepare to hand control over to kernel
    status = gBS->ExitBootServices(imageHandle, memoryMap.mapKey);
    RETURN_IF_ERROR_STATUS(status);

    // Recalculate kernel entry based on location in memory
    EntryPoint *KernelEntry = (EntryPoint *)((UINTN)kernel.start + (UINTN)kernel.entry);

    // Jump to kernel
    KernelEntry(kernel.start, kernel.pages, &memoryMap, &graphicsOutput, &font);

    // Kernel does not return so this point should never be reached
    return status;
}
