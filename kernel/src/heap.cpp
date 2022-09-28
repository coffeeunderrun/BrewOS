#include <heap.hpp>
#include <memory.hpp>

#define MAGIC 0xC0FFEE42

#define ATTR_FREE (1 << 0) // Block is available

#define SET_ATTR(header, attr) (header->attribute |= attr)
#define CLEAR_ATTR(header, attr) (header->attribute &= ~attr)

#define IS_FREE(header) (header != nullptr && (header->attribute & ATTR_FREE))
#define IS_VALID(header) (header != nullptr && (header->signature == MAGIC))

extern void *heap_start;
extern void *heap_end;

namespace BrewOS::Memory::Heap
{
    typedef struct BlockHeader
    {
        uint32_t signature;           // Block signature
        uint32_t attribute;           // Block attributes
        size_t size;                  // Length of block in bytes
        struct BlockHeader *next;     // Next block header
        struct BlockHeader *previous; // Previous block header
    } __attribute__((packed)) BlockHeader;

    static BlockHeader *GetFirstBlock();
    static BlockHeader *FindBestFitBlock(const size_t size);
    static BlockHeader *ExpandBlock(BlockHeader *header, const size_t size);
    static BlockHeader *ShrinkBlock(BlockHeader *header, const size_t size);
    static BlockHeader *GetBlockHeader(void *addr);
    static void *GetBlock(BlockHeader *header);
    static void MergeFreeBlocks(BlockHeader *header);

    void *Allocate(const size_t size)
    {
        BlockHeader *header = FindBestFitBlock(size);

        if (!IS_VALID(header))
        {
            return nullptr;
        }

        CLEAR_ATTR(header, ATTR_FREE);

        return GetBlock(ShrinkBlock(header, size));
    }

    void *Allocate(const size_t count, const size_t size)
    {
        void *p = Allocate(count * size);

        Memory::Zero(p, count * size);

        return p;
    }

    void *Resize(void *addr, const size_t size)
    {
        if (addr == nullptr)
        {
            return Allocate(size);
        }

        if (size == 0)
        {
            Free(addr);
            return nullptr;
        }

        BlockHeader *header = GetBlockHeader(addr);

        if (!IS_VALID(header))
        {
            return nullptr;
        }

        if (header->size > size + sizeof(BlockHeader))
        {
            header = ShrinkBlock(header, size);
        }
        else
        {
            header = ExpandBlock(header, size);
        }

        return GetBlock(header);
    }

    void Free(void *addr)
    {
        if (addr == nullptr)
        {
            return;
        }

        BlockHeader *header = GetBlockHeader(addr);

        if (!IS_VALID(header) || IS_FREE(header))
        {
            return;
        }

        SET_ATTR(header, ATTR_FREE);

        MergeFreeBlocks(header);
    }

    static BlockHeader *GetFirstBlock()
    {
        BlockHeader *header = reinterpret_cast<BlockHeader *>(&heap_start);

        if (header == nullptr)
        {
            return nullptr;
        }

        if (IS_VALID(header))
        {
            return header;
        }

        size_t size = reinterpret_cast<uint8_t *>(&heap_end) - reinterpret_cast<uint8_t *>(&heap_start);

        *header = {MAGIC, ATTR_FREE, size - sizeof(BlockHeader), nullptr, nullptr};

        return header;
    }

    static BlockHeader *FindBestFitBlock(const size_t size)
    {
        BlockHeader *best = nullptr;

        for (BlockHeader *header = GetFirstBlock(); IS_VALID(header); header = header->next)
        {
            if (!IS_FREE(header))
            {
                continue;
            }

            if (header->size < size)
            {
                continue;
            }

            if (best == nullptr || header->size < best->size)
            {
                best = header;
            }
        }

        return best;
    }

    static BlockHeader *ExpandBlock(BlockHeader *header, const size_t size)
    {
        if (!IS_VALID(header))
        {
            return nullptr;
        }

        if (IS_FREE(header))
        {
            return header;
        }

        size_t oldSize = header->size;

        // Attempt to fit new size by appending any following free blocks to the current block
        for (BlockHeader *next = header->next; IS_FREE(next) && size > header->size; next = next->next)
        {
            if (!IS_VALID(next))
            {
                return nullptr;
            }

            header->size += next->size + sizeof(BlockHeader);
            header->next = next->next;

            if (IS_VALID(header->next))
            {
                header->next->previous = header;
            }
        }

        if (header->size >= size)
        {
            return ShrinkBlock(header, size);
        }

        // Could not expand current block so try finding a new block
        void *newAddr = Allocate(size);
        if (newAddr == nullptr)
        {
            return nullptr;
        }

        // Copy old block to new block
        Memory::Move<uint8_t>(GetBlock(header), newAddr, oldSize);

        // Free up old block
        Free(GetBlock(header));

        return GetBlockHeader(newAddr);
    }

    static BlockHeader *ShrinkBlock(BlockHeader *header, const size_t size)
    {
        if (!IS_VALID(header))
        {
            return nullptr;
        }

        if (IS_FREE(header))
        {
            return header;
        }

        size_t remainder = header->size - size;

        if (size >= header->size || remainder < sizeof(BlockHeader))
        {
            return header;
        }

        // Split block if there is enough room for a header in a new block
        uint8_t *offset = reinterpret_cast<uint8_t *>(header) + size + sizeof(BlockHeader);

        BlockHeader *newHeader = reinterpret_cast<BlockHeader *>(offset);

        *newHeader = {MAGIC, ATTR_FREE, remainder - sizeof(BlockHeader), header->next, header};

        header->size = size;
        header->next = newHeader;

        MergeFreeBlocks(newHeader);

        return header;
    }

    static BlockHeader *GetBlockHeader(void *addr)
    {
        if (addr == nullptr)
        {
            return nullptr;
        }

        uint8_t *offset = static_cast<uint8_t *>(addr) - sizeof(BlockHeader);

        return reinterpret_cast<BlockHeader *>(offset);
    }

    static void *GetBlock(BlockHeader *header)
    {
        if (!IS_VALID(header))
        {
            return nullptr;
        }

        uint8_t *offset = reinterpret_cast<uint8_t *>(header) + sizeof(BlockHeader);

        return static_cast<void *>(offset);
    }

    static void MergeFreeBlocks(BlockHeader *header)
    {
        if (!IS_VALID(header) || !IS_FREE(header))
        {
            return;
        }

        while (IS_VALID(header->next) && IS_FREE(header->next))
        {
            header->size += header->next->size + sizeof(BlockHeader);
            header->next = header->next->next;

            if (IS_VALID(header->next))
            {
                header->next->previous = header;
            }
        }

        size_t size = header->size;
        for (BlockHeader *previous = header->previous; IS_VALID(previous) && IS_FREE(previous); previous = previous->previous)
        {
            size += previous->size + sizeof(BlockHeader);
            previous->size = size;
            previous->next = header->next;

            if (IS_VALID(header->next))
            {
                header->next->previous = previous;
            }
        }
    }
}
