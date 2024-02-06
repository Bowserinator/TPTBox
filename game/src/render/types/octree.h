#ifndef RENDER_OCTREE_H
#define RENDER_OCTREE_H

#include "stdint.h"
#include "../../simulation/SimulationGraphics.h"

#include <array>

namespace OctreeBlockMetadata {
    constexpr auto layer_offsets{[]() constexpr {
        std::array<unsigned int, OCTREE_BLOCK_DEPTH> result{};
        result[0] = 0;
        for (std::size_t layer = 1; layer < OCTREE_BLOCK_DEPTH; layer++)
            result[layer] = result[layer - 1] + (1 << (3 * layer - 3));
        return result;
    }() };

    constexpr unsigned int size = layer_offsets[OCTREE_BLOCK_DEPTH - 1] + (1 << (3 * OCTREE_BLOCK_DEPTH - 3));
}


class BitOctreeBlock {
public:
    BitOctreeBlock();
    ~BitOctreeBlock();

    BitOctreeBlock(const BitOctreeBlock &other) = delete;
    BitOctreeBlock &operator=(const BitOctreeBlock &other) = delete;

    /**
     * @brief Flag the location at x,y,z as occupied. This should be
     *        the location within the octree (ie 0 <= x,y,z < 2^DEPTH)
     *        Propagates changes to all layers
     * @param x x coordinate
     * @param y y coordinate
     * @param z z coordinate
     */
    void insert(uint8_t x, uint8_t y, uint8_t z);

    /**
     * @brief Flag the location at x,y,z as unoccupied. This should be
     *        the location within the octree (ie 0 <= x,y,z < 2^DEPTH)
     *        Propagates changes to all layers
     * @param x x coordinate
     * @param y y coordinate
     * @param z z coordinate
     */
    void remove(uint8_t x, uint8_t y, uint8_t z);

    // Stored as follows: let layer 0 = top most (root) node
    // The data is stored packed as [layer0][layer1][layer2]...[layer depth-1]
    // Each byte is a bitmask of which children are occupied, numbered in xyz norton order
    // (so 0,0,0 = 0, 1,1,1 = 7, 0,1,1 = 3, etc... interweaving the bits from MSB to LSB)

    // If (x,y,z) is coordinates for a specific voxel (ie 0 <= x,y,z < 2^DEPTH)
    // then the 2x2x2 group the voxel is in for a given layer is:
    // layer_offsets[layer] + (morton_code(x, y, z) >> (3 * (depth - layer)))
    // and the corresponding bit for it is the last 3 bits of morton_code(x, y, z) >> (3 * (depth - layer - 1))
    uint8_t * data;

    uint8_t modified = 0x0;
};

#endif