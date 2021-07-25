#ifndef BIT_UTIL_HPP
#define BIT_UTIL_HPP

#include <cstdint>

namespace utils
{
    inline uint8_t bits_needed(uint32_t value)
    {
        uint8_t bits = 0U;
        for (uint32_t bit_test = 16U; bit_test > 0U; bit_test >>= 1U)
        {
            if (value >> bit_test != 0U)
            {
                bits += static_cast<uint8_t>(bit_test);
                value >>= bit_test;
            }
        }
        return bits + static_cast<uint8_t>(value);
    }

    inline bool IsBitSet(uint32_t value, uint32_t bit)
    {
        return (value & (1 << bit));
    }
}

#endif