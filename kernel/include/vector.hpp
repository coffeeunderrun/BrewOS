#ifndef BREWOS_VECTOR_H
#define BREWOS_VECTOR_H

#include <heap.hpp>
#include <memory.hpp>
#include <stddef.h>

namespace BrewOS
{
    template <class T>
    class Vector
    {
    private:
        T *m_data = nullptr;

        size_t m_capacity, m_count;

    public:
        Vector(size_t capacity = 1)
            : m_capacity(capacity)
        {
            m_data = static_cast<T *>(Memory::Heap::Allocate(capacity * sizeof(T)));

            for (size_t i = 0; i < capacity; i++)
            {
                m_data[i] = T{};
            }
        }

        ~Vector()
        {
            Memory::Heap::Free(m_data);
        }

        size_t GetCapacity()
        {
            return m_capacity;
        }

        size_t GetCount()
        {
            return m_count;
        }

        T *begin()
        {
            return static_cast<T *>(m_data);
        }

        T *end()
        {
            return static_cast<T *>(m_data + m_count);
        }

        void Push(T item)
        {
            if (m_count == m_capacity && !Expand())
            {
                // Out of memory
                return;
            }

            m_data[m_count++] = item;
        }

        T Pop()
        {
            if (m_count == 0)
            {
                // No items
                return T{};
            }

            if (m_count <= m_capacity / 4)
            {
                // Reduce capacity if count reaches quarter of current capacity
                Shrink();
            }

            return m_data[--m_count];
        }

        void Insert(T item, size_t index)
        {
            if (index >= m_count)
            {
                // Out of bounds
                return;
            }

            if (m_count == m_capacity && !Expand())
            {
                // Out of memory
                return;
            }

            Memory::Move<T>(&m_data[index], &m_data[index + 1], m_count - index);

            m_data[index] = item;
            m_count++;
        }

        T Remove(size_t index)
        {
            if (index >= m_count)
            {
                // Out of bounds
                return T{};
            }

            if (m_count <= m_capacity / 4)
            {
                // Reduce capacity if count reaches quarter of current capacity
                Shrink();
            }

            T item = m_data[index];

            Memory::Move<T>(&m_data[index + 1], &m_data[index], m_count - index);

            m_count--;

            return item;
        }

        void Set(T item, size_t index)
        {
            if (index >= m_count)
            {
                // Out of bounds
                return;
            }

            m_data[index] = item;
        }

        T Get(size_t index)
        {
            if (index >= m_count)
            {
                // Out of bounds
                return T{};
            }

            return m_data[index];
        }

    private:
        bool Expand()
        {
            size_t newCapacity = m_capacity * 2;

            T *data = static_cast<T *>(Memory::Heap::Resize(m_data, newCapacity * sizeof(T)));

            if (data == nullptr)
            {
                // Out of memory
                return false;
            }

            m_data = data;
            m_capacity = newCapacity;

            for (size_t i = m_count; i < newCapacity; i++)
            {
                m_data[i] = T{};
            }

            return true;
        }

        void Shrink()
        {
            m_capacity /= 2;
            m_data = static_cast<T *>(Memory::Heap::Resize(m_data, m_capacity * sizeof(T)));
        }
    };
}

#endif // BREWOS_VECTOR_H
