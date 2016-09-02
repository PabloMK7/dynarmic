/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * This software may be used and distributed according to the terms of the GNU
 * General Public License version 2 or any later version.
 */

#pragma once

#include <climits>

#include "common/assert.h"
#include "common/common_types.h"

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace Dynarmic {
namespace Common {

/// The size of a type in terms of bits
template<typename T>
constexpr size_t BitSize() {
    return sizeof(T) * CHAR_BIT;
}

/// Extract bits [begin_bit, end_bit] inclusive from value of type T.
template<size_t begin_bit, size_t end_bit, typename T>
constexpr T Bits(const T value) {
    static_assert(begin_bit < end_bit,
                  "invalid bit range (position of beginning bit cannot be greater than that of end bit)");
    static_assert(begin_bit < BitSize<T>(), "begin_bit must be smaller than size of T");
    static_assert(end_bit < BitSize<T>(), "begin_bit must be smaller than size of T");

    return (value >> begin_bit) & ((1 << (end_bit - begin_bit + 1)) - 1);
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4554)
#endif
/// Extracts a single bit at bit_position from value of type T.
template<size_t bit_position, typename T>
constexpr bool Bit(const T value) {
    static_assert(bit_position < BitSize<T>(), "bit_position must be smaller than size of T");

    return ((value >> bit_position) & 1) != 0;
}

/// Extracts a single bit at bit_position from value of type T.
template<typename T>
constexpr bool Bit(size_t bit_position, const T value) {
    ASSERT_MSG(bit_position < BitSize<T>(), "bit_position must be smaller than size of T");

    return ((value >> bit_position) & 1) != 0;
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

/// Sign-extends a value that has NBits bits to the full bitwidth of type T.
template<size_t bit_count, typename T>
inline T SignExtend(const T value) {
    static_assert(bit_count <= BitSize<T>(), "bit_count larger than bitsize of T");

    constexpr T mask = static_cast<T>(1ULL << bit_count) - 1;
    const bool signbit = Bit<bit_count - 1, T>(value);
    if (signbit) {
        return value | ~mask;
    }
    return value;
}

inline size_t BitCount(u32 value) {
    // Portable SWAR algorithm for population count
    value = value - ((value >> 1) & 0x55555555);                // Two-bit count
    value = (value & 0x33333333) + ((value >> 2) & 0x33333333); // Nybble count
    value = (value + (value >> 4)) & 0x0F0F0F0F;                // Byte count
    return ((value * 0x01010101) >> 24) & 0xFF;                 // Summate the byte counts
}

} // namespace Common
} // namespace Dynarmic
