#include <Common.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/MemoryMapLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS GetMemoryMap(MemoryMap *memoryMap)
{
    ASSERT(memoryMap != NULL);

    EFI_STATUS status;

    memoryMap->mapSize = sizeof(EFI_MEMORY_DESCRIPTOR);

    do
    {
        status = gBS->AllocatePool(EfiLoaderData, memoryMap->mapSize, (VOID **)&memoryMap->map);
        RETURN_IF_ERROR_STATUS(status);

        status = gBS->GetMemoryMap(&memoryMap->mapSize, memoryMap->map, &memoryMap->mapKey, &memoryMap->descriptorSize, &memoryMap->descriptorVersion);
        if (status == EFI_BUFFER_TOO_SMALL)
        {
            memoryMap->mapSize += memoryMap->descriptorSize;
            FreePool(memoryMap->map);
        }
        else
        {
            RETURN_IF_ERROR_STATUS(status);
        }
    } while (status != EFI_SUCCESS);

    return status;
}
