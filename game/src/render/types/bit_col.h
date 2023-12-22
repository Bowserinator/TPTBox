#ifndef BIT_COL_H
#define BIT_COL_H

#include <bit>
#include <cstdint>
#include <vector>
#include <algorithm>

using BitColY = unsigned short;

class BitColumn {
public:
    // bytes[0] corresponds y 0-7
    // bytes[0] leftmost bit corresponds to y 7
    std::vector<std::uint8_t> bytes;

    bool getY(const BitColY y) const {
        return (bytes[y / 8] & (1 << (y % 8))) > 0;
    }

    void setY(const BitColY y) {
        bytes[y / 8] |= (1 << (y % 8));
    }

    void unsetY(const BitColY y) {
        bytes[y / 8] &= ~(1 << (y % 8));
    }

    void unsetYRange(const BitColY y1, const BitColY y2) {
        unsigned int start_byte = y1 / 8;
        unsigned int end_byte = y2 / 8;

        if (start_byte == end_byte) {
            int mask = ((1 << (y2 % 8 + 1)) - 1) ^ ((1 << ((y1 % 8 + 1) - 1)) - 1);
            bytes[start_byte] &= ~mask;
            return;
        }

        bytes[start_byte] &= (1 << (y1 % 8)) - 1;
        for (unsigned int byte = start_byte + 1; byte < end_byte; byte++)
            bytes[byte] = 0x00;
        bytes[end_byte] &= ~((1 << (y2 % 8)) - 1);
    }

    bool isRangeAllOne(const BitColY start, const BitColY end) const {
        // If start byte and end byte are the same
        unsigned int start_byte = start / 8;
        unsigned int end_byte = end / 8;
        
        if (start_byte == end_byte) {
            for (int idx = start % 8; idx <= end % 8; idx++) {
                if ((bytes[start_byte] & (1 << idx)) == 0)
                    return false;
            }
            return true;
        }
        
        // If start byte != end byte: check prefix and suffix
        // and in between is all 0xFF
        if (std::countl_one(bytes[start_byte]) != 8 - start % 8)
            return false;
        if (std::countr_one(bytes[end_byte]) != end % 8 + 1)
            return false;
        for (unsigned int byte = start_byte + 1; byte < end_byte; byte++)
            if (bytes[byte] != 0xFF)
                return false;
        return true;
    }

    BitColY getContigiousEnd(const BitColY start_t) const {
        unsigned int start_byte = start_t / 8;
        unsigned int offset = start_t % 8;
        
        // Cases:
        // 1. End y is on the current byte
        //    0b011100..
        // start ^ ^ end
        unsigned int suffix_ones_on_start_byte = 0;
        while (offset < 8 && (bytes[start_byte] & (1 << offset))) {
            suffix_ones_on_start_byte++;
            offset++;
        }

        // Case 1
        if (start_byte == bytes.size() - 1 || suffix_ones_on_start_byte < 8 - (start_t % 8))
            return start_t + suffix_ones_on_start_byte - 1;
    
        // 2. Current byte starting from start_t ends in all 1s ie
        //    0b00011111 0b11111111 0b110...
        //         ^ start here        ^ end here
        //    In this case we can skip 0xFF bytes and
        //    quickly count left ones of the last byte we stop at
        unsigned int end_byte = start_byte + 1;
        while (bytes[end_byte] == 0xFF && end_byte < bytes.size() - 1)
            end_byte++;
        return end_byte * 8 + std::countr_one(bytes[end_byte]) - 1;
    }
};

#endif