#ifndef UTIL_BITSET8
#define UTIL_BITSET8

#include "stdint.h"
#include <cstddef>
#include <string>

namespace util {

class Bitset8 {
public:
    struct reference {
        uint8_t &data;
        std::size_t pos;

        reference(uint8_t &data, std::size_t pos): data(data), pos(pos) {}
        reference &operator=(bool x) noexcept {
            data = x ? (data | (1 << pos)) : (data & ~(1 << pos));
            return *this;
        }
        bool operator~() const { return !(data & (1 << pos)); }
        operator bool() const { return (data >> pos) & 1; }

        void flip() { *this = !*this; }
    };

    Bitset8(): data(0) {}
    Bitset8(uint8_t data): data(data) {}

    std::string to_string() {
        std::string out(size(), ' ');
        for (int i = 0; i < size(); i++)
            out[i] = (*this)[i] ? '1' : '0';
        return out;
    }

    inline friend bool operator==(const Bitset8 &left, const Bitset8 &right) { return left.data == right.data; }
    inline friend bool operator==(const Bitset8 &left, const uint8_t right) { return left.data == right; }
    inline friend bool operator==(const uint8_t left, const Bitset8 &right) { return left == right.data; }

    Bitset8 &operator&=(const Bitset8& other) { data &= other.data; return *this; }
    Bitset8 &operator|=(const Bitset8& other) { data |= other.data; return *this; }
    Bitset8 &operator^=(const Bitset8& other) { data ^= other.data; return *this; }
    Bitset8 operator~() const { return Bitset8(~data); }

    Bitset8 operator<<(std::size_t pos) const { return Bitset8(data << pos); }
    Bitset8 &operator<<=(std::size_t pos) { data <<= pos; return *this; }
    Bitset8 operator>>(std::size_t pos) const { return Bitset8(data >> pos); }
    Bitset8 &operator>>=(std::size_t pos) { data >>= pos; return *this; }

    inline friend Bitset8 operator&(const Bitset8 &left, const Bitset8 &right) {
        return Bitset8(left.data & right.data);
    }
    inline friend Bitset8 operator^(const Bitset8 &left, const Bitset8 &right) {
        return Bitset8(left.data ^ right.data);
    }
    inline friend Bitset8 operator|(const Bitset8 &left, const Bitset8 &right) {
        return Bitset8(left.data | right.data);
    }

    // Note: differs from std::bitset in that set cannot take a value for what to set
    void set() { data = 0xFF; }
    void set(std::size_t pos) { data |= 1 << pos; }
    void unset() { data = 0; }
    void unset(std::size_t pos) { data &= ~(1 << pos); }
    void reset() { reset(); }

    operator uint8_t() const { return data; }

    bool operator[](std::size_t pos) const { return (data >> pos) & 1; }
    reference operator[](std::size_t pos) { return reference(data, pos); }

    bool all() const noexcept { return data == 0xFF; }
    bool any() const noexcept { return data != 0; }
    bool none() const noexcept { return data == 0; }

    constexpr std::size_t size() const { return 8; }
private:
    uint8_t data;
};

}

#endif