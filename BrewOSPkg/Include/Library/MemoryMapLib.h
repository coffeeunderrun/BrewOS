#ifndef MEMORY_MAP_LIB_H
#define MEMORY_MAP_LIB_H

#include <Uefi.h>

typedef struct MemoryMap
{
    EFI_MEMORY_DESCRIPTOR *map;
    UINTN mapSize;
    UINTN mapKey;
    UINTN descriptorSize;
    UINT32 descriptorVersion;
} MemoryMap;

EFI_STATUS GetMemoryMap(MemoryMap *memoryMap);

#endif // MEMORY_MAP_LIB_H
