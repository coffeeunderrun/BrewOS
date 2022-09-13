#ifndef BREWOS_MEMORY_H
#define BREWOS_MEMORY_H

#include <stddef.h>
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

    typedef struct MemoryMap
    {
        MemoryMapEntry *map;
        uint64_t mapSize;
        uint64_t mapKey;
        uint64_t descriptorSize;
        uint32_t descriptorVersion;
    } MemoryMap;

    void Initialize();

    void Zero(void *addr, const size_t size);

    template <class T>
    int Compare(const void *addr1, const void *addr2, const size_t size)
    {
        T *p1 = static_cast<T *>(const_cast<void *>(addr1));
        T *p2 = static_cast<T *>(const_cast<void *>(addr2));

        for (size_t i = 0; i < size; i++)
        {
            if (*p1++ != *p2++)
            {
                break;
            }
        }

        return *p1 - *p2;
    }

    template <class T>
    void Copy(const void *src, void *dest, const size_t size)
    {
        T *ps = static_cast<T *>(const_cast<void *>(src));
        T *pd = static_cast<T *>(dest);

        for (size_t i = 0; i < size; i++)
        {
            *pd++ = *ps++;
        }
    }

    template <class T>
    void *Find(const void *addr, const T value, const size_t size)
    {
        T *p = static_cast<T *>(const_cast<void *>(addr));

        for (size_t i = 0; i < size; i++)
        {
            if (*p++ == value)
            {
                return p;
            }
        }

        return nullptr;
    }

    template <class T>
    void Move(void *src, void *dest, const size_t size)
    {
        if (dest < src)
        {
            Copy<T>(src, dest, size);
        }

        T *ps = static_cast<T *>(src) + size - 1;
        T *pd = static_cast<T *>(dest) + size - 1;

        for (size_t i = 0; i < size; i++)
        {
            *pd-- = *ps--;
        }
    }

    template <class T>
    void Set(void *addr, const T value, const size_t size)
    {
        T *p = static_cast<T *>(addr);

        for (size_t i = 0; i < size; i++)
        {
            *p++ = value;
        }
    }

    template <class T>
    void Set(void *addr, const T value, const size_t count, const size_t size)
    {
        uint8_t *p = static_cast<uint8_t *>(addr);

        for (size_t i = 0; i < count; i++)
        {
            *reinterpret_cast<T *>(p) = value;
            p += size;
        }
    }
}

#endif // BREWOS_MEMORY_H
