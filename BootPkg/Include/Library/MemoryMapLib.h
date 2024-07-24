#pragma once

#include <Uefi.h>

typedef struct MemoryMap {
    EFI_MEMORY_DESCRIPTOR* map;
    UINTN mapSize;
    UINTN mapKey;
    UINTN descriptorSize;
    UINT32 descriptorVersion;
} MemoryMap;

/// @brief
/// @param memoryMap
/// @return
EFI_STATUS GetMemoryMap(MemoryMap* memoryMap);
