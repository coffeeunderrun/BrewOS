#ifndef MEMORYMAPLIB_H
#define MEMORYMAPLIB_H

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

#endif // MEMORYMAPLIB_H
