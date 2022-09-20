#ifndef BREWOS_LINKEDLIST_H
#define BREWOS_LINKEDLIST_H

namespace BrewOS
{
    template <class T>
    class LinkedList
    {
    private:
        typedef struct Node
        {
            T data;
            struct Node *next;
            struct Node *previous;
        } Node;

        Node *m_start = nullptr,
             *m_end = nullptr;

    public:
        ~LinkedList()
        {
            for (Node *node = m_start; node != nullptr; node = node->next)
            {
                delete node;
            }
        }

        void Add(T item)
        {
            Node *node = new Node{item, nullptr, m_end};

            if (m_start == nullptr)
            {
                m_start = node;
            }

            if (m_end != nullptr)
            {
                m_end->next = node;
            }

            m_end = node;
        }

        void Remove(T item)
        {
            for (Node *node = m_start; node != nullptr; node = node->next)
            {
                if (node->data != item)
                {
                    continue;
                }

                if (node->previous != nullptr)
                {
                    node->previous->next = node->next;
                }

                if (node->next != nullptr)
                {
                    node->next->previous = node->previous;
                }

                delete node;
            }
        }

        class Iterator
        {
        private:
            const Node *m_current;

        public:
            Iterator() : m_current(nullptr)
            {
            }

            Iterator(const Node *node) : m_current(node)
            {
            }

            T operator*()
            {
                if (m_current == nullptr)
                {
                    return T{};
                }
                return m_current->data;
            }

            bool operator==(const Iterator &iterator)
            {
                return m_current == iterator.m_current;
            }

            bool operator!=(const Iterator &iterator)
            {
                return m_current != iterator.m_current;
            }

            Iterator &operator=(Node *node)
            {
                m_current = node;
                return *this;
            }

            Iterator &operator++()
            {
                if (m_current != nullptr)
                {
                    m_current = m_current->next;
                }
                return *this;
            }

            Iterator operator++(int)
            {
                Iterator iterator = *this;
                ++*this;
                return iterator;
            }

            Iterator &operator--()
            {
                if (m_current != nullptr)
                {
                    m_current = m_current->previous;
                }
                return *this;
            }

            Iterator operator--(int)
            {
                Iterator iterator = *this;
                --*this;
                return iterator;
            }
        };

        Iterator begin()
        {
            return Iterator(m_start);
        }

        Iterator end()
        {
            return Iterator(nullptr);
        }
    };
}

#endif // BREWOS_LINKEDLIST_H
