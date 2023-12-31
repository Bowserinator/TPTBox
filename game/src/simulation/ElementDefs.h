#ifndef ELEMENT_DEFS_H
#define ELEMENT_DEFS_H

constexpr unsigned short PT_NUM = 1024; // TODO should be max possible

using ElementState = unsigned int;

constexpr ElementState TYPE_SOLID = 0x1;
constexpr ElementState TYPE_POWDER = 0x2;
constexpr ElementState TYPE_LIQUID = 0x4;
constexpr ElementState TYPE_GAS = 0x8;
constexpr ElementState TYPE_ENERGY = 0x10;

#endif