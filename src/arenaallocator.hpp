#pragma once

class ArenaAllocator {
    public:
        inline ArenaAllocator(const size_t bytes) : m_size(bytes) {
            m_buffer = static_cast<std::byte *>(malloc(m_size));
            m_offset = m_buffer;
        }

        ArenaAllocator(const ArenaAllocator&) = delete;
        ArenaAllocator& operator=(const ArenaAllocator&) = delete;

        template <typename name> inline name* alloc()
        {
            void *offset = m_offset;
            m_offset += sizeof(name);

            return static_cast<name*>(offset);
        }

        ~ArenaAllocator() {
            free(m_buffer);
        }

    private:
        size_t m_size;
        std::byte* m_buffer;
        std::byte* m_offset;
};