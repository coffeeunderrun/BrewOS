#ifndef BREWOS_MEMORY_H
#define BREWOS_MEMORY_H

#include <stdint.h>

namespace BrewOS::Memory
{
    typedef struct MemoryMapEntry
    {
        uint32_t type;
        uint32_t ignore;
        uint64_t physicalBase;
        uint64_t virtualBase;
        uint64_t pageCount;
        uint64_t attributes;
    } MemoryMapEntry;

    typedef struct MemoryMapInfo
    {
        MemoryMapEntry *map;
        uint64_t mapSize;
        uint64_t mapKey;
        uint64_t descriptorSize;
        uint32_t descriptorVersion;
    } MemoryMapInfo;
}

#endif // BREWOS_MEMORY_H
