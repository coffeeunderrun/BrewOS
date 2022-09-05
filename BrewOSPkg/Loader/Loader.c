#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/ElfLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>

#define RETURN_IF_ERROR_STATUS(status)                                      \
    if (EFI_ERROR(status))                                                  \
    {                                                                       \
        DEBUG((DEBUG_ERROR, "%a %d %d\n", __FUNCTION__, __LINE__, status)); \
        return status;                                                      \
    }

typedef struct KernelInfo
{
    VOID *entry;
    VOID *top;
    UINTN pages;
} KernelInfo;

typedef struct MemoryMapInfo
{
    EFI_MEMORY_DESCRIPTOR *map;
    UINTN mapSize;
    UINTN mapKey;
    UINTN descriptorSize;
    UINT32 descriptorVersion;
} MemoryMapInfo;

typedef __attribute__((sysv_abi)) VOID *EntryPoint(KernelInfo *kernelInfo, MemoryMapInfo *memoryMapInfo);

EFI_STATUS GetMemoryMap(MemoryMapInfo *memoryMapInfo);

EFI_STATUS EFIAPI UefiEntry(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable)
{
    EFI_STATUS status;

    UINTN loaderPages;
    VOID *loaderEntry, *loaderTop;
    status = LoadElfImage(L"loader.elf", imageHandle, &loaderEntry, &loaderTop, &loaderPages);
    if (EFI_ERROR(status))
    {
        Print(L"Unable to load LOADER.\n");
        return status;
    }

    UINTN kernelPages;
    VOID *kernelEntry, *kernelTop;
    status = LoadElfImage(L"kernel.elf", imageHandle, &kernelEntry, &kernelTop, &kernelPages);
    if (EFI_ERROR(status))
    {
        Print(L"Unable to load KERNEL.\n");
        return status;
    }

    MemoryMapInfo memoryMapInfo;
    status = GetMemoryMap(&memoryMapInfo);
    RETURN_IF_ERROR_STATUS(status);

    status = gBS->ExitBootServices(imageHandle, memoryMapInfo.mapKey);
    RETURN_IF_ERROR_STATUS(status);

    // Recalculate loader entry based on location in memory
    EntryPoint *_loader = (EntryPoint *)((UINTN)loaderEntry + (UINTN)loaderTop);

    // Do not bother recalculating kernel entry point as the loader will handle mapping to higher half
    KernelInfo kernelInfo = {kernelEntry, kernelTop, kernelPages};

    _loader(&kernelInfo, &memoryMapInfo);

    return status;
}

EFI_STATUS GetMemoryMap(MemoryMapInfo *memoryMapInfo)
{
    ASSERT(memoryMapInfo != NULL);

    EFI_STATUS status;

    memoryMapInfo->mapSize = sizeof(EFI_MEMORY_DESCRIPTOR);

    do
    {
        status = gBS->AllocatePool(EfiLoaderData, memoryMapInfo->mapSize, (VOID **)&memoryMapInfo->map);
        RETURN_IF_ERROR_STATUS(status);

        status = gBS->GetMemoryMap(&memoryMapInfo->mapSize, memoryMapInfo->map, &memoryMapInfo->mapKey, &memoryMapInfo->descriptorSize, &memoryMapInfo->descriptorVersion);
        if (status == EFI_BUFFER_TOO_SMALL)
        {
            memoryMapInfo->mapSize += memoryMapInfo->descriptorSize;
            FreePool(memoryMapInfo->map);
        }
        else
        {
            RETURN_IF_ERROR_STATUS(status);
        }
    } while (status != EFI_SUCCESS);

    return status;
}