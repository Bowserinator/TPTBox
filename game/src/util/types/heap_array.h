#ifndef UTIL_HEAP_ARRAY_H
#define UTIL_HEAP_ARRAY_H

#include <cstddef>
#include <stdexcept>
#include <algorithm>

namespace util {
    /**
     * @brief A heap allocated variant of std::array
     * 
     * @tparam T Type of array
     * @tparam N Length of array
     */
    template <class T, std::size_t N>
    class heap_array {
    public:
        heap_array(): _size(0), _data(nullptr) {
            _data = new T[N];
            _size = N;
        }

        ~heap_array() { _destroy(); }

        heap_array(const heap_array<T, N> &other): _size(0), _data(nullptr) {
            _data = new T[N];
            _size = N;
            std::copy(&other._data[0], &other._data[N], _data);
        }

        heap_array &operator=(heap_array<T, N> other) noexcept {
            std::swap(*this, other);
            return *this;
        }

        heap_array(heap_array<T, N> &&other) noexcept: _size(0), _data(nullptr) {
            if (this != &other) swap(other);
        }

        heap_array &operator=(heap_array<T, N> &&other) noexcept {
            if (this != &other) swap(other);
            return *this;
        }

        T& at(std::size_t n) {
            if (n >= size()) throw std::out_of_range("Array index out of range");
            return _data[n];
        }
        T& operator[](std::size_t n) { return _data[n];}
        T& front() { return _data[0]; }
        T& back() { return _data[_size - 1]; }

        const T& at(std::size_t n) const {
            if (n >= size()) throw std::out_of_range("Array index out of range");
            return _data[n];
        }
        const T& operator[](std::size_t n) const { return _data[n];}
        const T& front() const { return _data[0]; }
        const T& back() const { return _data[_size - 1]; }

        T* data() noexcept { return _data; }

        std::size_t size() const noexcept { return _size; }
        bool empty() const noexcept { return _size == 0; }
        
        void fill(const T& value) {
            std::fill(&_data[0], &_data[N], value);
        }

        void swap(heap_array<T, N> &other) noexcept {
            std::swap(other._data, _data);
            std::swap(other._size, _size);
        }
    private:
        T * _data;
        std::size_t _size;

        void _destroy() {
            delete[] _data;
            _size = 0;
            _data = nullptr;
        }
    };
}

#endif