#ifndef UTIL_SPINLOCK_H
#define UTIL_SPINLOCK_H

#include <atomic>

namespace util {
    class Spinlock {
    public:
        Spinlock() {}
        Spinlock(const Spinlock &) = delete;
        Spinlock& operator=(const Spinlock&) = delete;

        inline void lock() {
            while (_lock.test_and_set(std::memory_order_acquire)) {
            #if defined(__cpp_lib_atomic_flag_test)
                while (_lock.test(std::memory_order_relaxed)) // C++20, otherwise just busy wait
                    __builtin_ia32_pause();
            #endif
            }
        }

        // Try lock, and return if successful
        inline bool try_lock() { return !_lock.test_and_set(std::memory_order_acquire); }
        inline void unlock() { _lock.clear(std::memory_order_release); }
    private:
        std::atomic_flag _lock = ATOMIC_FLAG_INIT;
    };

    // Dead simple RAII implementation for Spinlock
    // Usage: util::unqiue_spinlock tmp(myLock);
    // Automatically locks on creation (blocking) and unlocks on destruction
    class unique_spinlock {
    public:
        unique_spinlock(Spinlock &lock): _lock(&lock) { _lock->lock(); }
        ~unique_spinlock() { unlock(); }

        void unlock() {
            if (!_lock) return;
            _lock->unlock();
            _lock = nullptr;
        }

        unique_spinlock(const unique_spinlock &other) = delete;
        unique_spinlock &operator=(const unique_spinlock &other) = delete;
    private:
        Spinlock * _lock;
    };
}

#endif