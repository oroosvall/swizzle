
/* Include files */

#include "ChannelCommon.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace swm::channel
{
    U64 GetChannelElemetSize(U8 channelFlags)
    {
        const U64 lut[] = {1ull, 2ull, 4ull, 8ull};
        U8 typeBits = channelFlags & 0x03;

        return lut[typeBits];
    }
} // namespace swm::channel

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
