#ifndef UTIL_TYPES_CONCURRENT_APPEND_LIST_H_
#define UTIL_TYPES_CONCCURENT_APPEND_LIST_H_

#include <cstring>
#include <atomic>
#include <cmath>

namespace util {
    template <class T, std::size_t N, std::size_t BLOCK_SIZE, std::size_t THREADS>
    class ConcurrentAppendList {
    public:
        /**
         * @brief Add an item into the list from a given thread
         * @param thread_id 0 <= thread_id < THREADS
         * @param val Value to insert
         */
        void append(const std::size_t thread_id, T val) {
            auto &block = m_block_data[thread_id];
            if (block.block_idx >= BLOCK_SIZE) {
                block.head_block_id = m_next_free++;
                block.block_idx = 0;
            }
            m_data[block.head_block_id * BLOCK_SIZE + (block.block_idx++)] = val;
        }

        /**
         * @brief Add a terminating sentinel value to the end of each partially filled block
         * @param terminating_sentinel_val Terminating value to cap each block off with
         */
        void cap_partial_blocks(const T terminating_sentinel_val) {
            if (m_next_free == 0) return;

            for (auto thread_id = 0; thread_id < THREADS; thread_id++) {
                auto &block = m_block_data[thread_id];
                if (block.block_idx < BLOCK_SIZE)
                    m_data[block.head_block_id * BLOCK_SIZE + (block.block_idx++)] = terminating_sentinel_val;
            }
        }

        /** @brief Reset the list (can think of as a clear) */
        void reset() {
            m_next_free = 0;
            for (auto i = 0; i < THREADS; i++) {
                m_block_data[i].head_block_id = 0;
                m_block_data[i].block_idx = BLOCK_SIZE;
            }
        }

        struct ThreadHead {
            std::size_t head_block_id = 0;
            std::size_t block_idx = BLOCK_SIZE;
        };

        constexpr std::size_t capacity() const noexcept { return N; }
        constexpr std::size_t block_size() const noexcept { return BLOCK_SIZE; }

        T * data() noexcept { return m_data; }
        std::size_t size() const noexcept { return m_next_free * BLOCK_SIZE; }
        bool empty() const noexcept { return size() == 0; }

        T& operator[](std::size_t pos) { return m_data[pos]; }
        const T& operator[](std::size_t pos) const { return m_data[pos]; }

    private:
        T m_data[N];
        ThreadHead m_block_data[THREADS];
        std::atomic<std::size_t> m_next_free = 0;
    };
}

#endif // UTIL_TYPES_CONCURRENT_APPEND_LIST_H_
