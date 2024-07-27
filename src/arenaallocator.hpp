#pragma once

#include <cstddef>
#include <memory>
#include <utility>

class ArenaAllocator {
    public:
        explicit ArenaAllocator(size_t max_num_bytes) : m_size { max_num_bytes }
                                                      , m_buffer { new std::byte[max_num_bytes] }
                                                      , m_offset { m_buffer } {

        }
        ArenaAllocator(const ArenaAllocator&) = delete;
        ArenaAllocator& operator=(const ArenaAllocator&) = delete;

        ArenaAllocator(ArenaAllocator&& other) noexcept : m_size { std::exchange(other.m_size, 0) }
                                                        , m_buffer { std::exchange(other.m_buffer, nullptr) }
                                                        , m_offset { std::exchange(other.m_offset, nullptr) } {

        }

        ArenaAllocator& operator=(ArenaAllocator&& other) noexcept {
            std::swap(m_size, other.m_size);
            std::swap(m_buffer, other.m_buffer);
            std::swap(m_offset, other.m_offset);
            return *this;
        }

        template <typename type> [[nodiscard]] type* alloc()
        {
            size_t remaining_num_bytes = m_size - static_cast<size_t>(m_offset - m_buffer);
            auto pointer = static_cast<void*>(m_offset);
            const auto aligned_address = std::align(alignof(type), sizeof(type), pointer, remaining_num_bytes);
            if (aligned_address == nullptr) {
                throw std::bad_alloc {};
            }
            m_offset = static_cast<std::byte*>(aligned_address) + sizeof(type);
            return static_cast<type*>(aligned_address);
        }

        template <typename type, typename... Args> [[nodiscard]] type* emplace(Args&&... args) {
            const auto allocated_memory = alloc<type>();
            return new (allocated_memory) type { std::forward<Args>(args)... };
        }


        ~ArenaAllocator() {
            delete[] m_buffer;
        }

    private:
        size_t m_size{};
        std::byte* m_buffer{};
        std::byte* m_offset{};
};