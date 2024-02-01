#include "octree.h"
#include "../../util/morton.h"

BitOctreeBlock::BitOctreeBlock() {
    // TODO: pre compute layer offsets somewhere
    layer_offsets[0] = 0;
    for (int layer = 1; layer < OCTREE_BLOCK_DEPTH; layer++)
        layer_offsets[layer] = layer_offsets[layer - 1] + (1 << (3 * layer - 3));
    size = layer_offsets[OCTREE_BLOCK_DEPTH - 1] + (1 << (3 * OCTREE_BLOCK_DEPTH - 3));
    data = new uint8_t[size]();
}

BitOctreeBlock::~BitOctreeBlock() {
    delete[] data;
    data = nullptr;
}

void BitOctreeBlock::insert(uint8_t x, uint8_t y, uint8_t z) {
    uint32_t morton = util::morton_decode8(x, y, z);
    unsigned int bit_idx = morton & 0b111; // % 8
    morton >>= 3; // Move to the layer of 2x2x2 cubes
    
    // Layer OCTREE_BLOCK_DEPTH = 1x1x1 voxels
    // We start at the layer above (2x2x2) and write the bit to signify which
    // subvoxel (starting at size 1x1x1) is set, then go up to level 0 (the 64x64x64 block)
    for (int layer = OCTREE_BLOCK_DEPTH - 1; layer >= 0; layer--) {
        unsigned int idx = morton + layer_offsets[layer];
        bool early_exit = data[idx] != 0x0; // If the current layer is filled already no need to update parent

        data[idx] |= 1 << bit_idx;
        bit_idx = morton & 0b111; // % 8
        morton >>= 3;

        if (early_exit)
            break;
    }
}

void BitOctreeBlock::remove(uint8_t x, uint8_t y, uint8_t z) {
    uint32_t morton = util::morton_decode8(x, y, z);
    unsigned int bit_idx = morton & 0b111; // % 8
    morton >>= 3;

    for (int layer = OCTREE_BLOCK_DEPTH - 1; layer >= 0; layer--) {
        unsigned int idx = morton + layer_offsets[layer];
        data[idx] &= ~(1 << bit_idx);
        if (data[idx] != 0x0)
            break;

        bit_idx = morton & 0b111; // % 8
        morton >>= 3;
    }
}
