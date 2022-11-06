#ifndef CHANNEL_COMMON_HPP
#define CHANNEL_COMMON_HPP

/* Include files */

#include "InternalStructs.hpp"
#include <utils/Bitstream.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swm::channel
{
    struct CompressedChannel
    {
        U8 mChannelAttribute;
        U8 mChannelDataFlags;
        U32 mElementCount;
        std::vector<U8> mData;
        U8 mBitsPerIndex;
        U32 mVertexCount;
        std::vector<U8> mBitData;
    };
} // namespace swm::channel

/* Class Declaration */

/* Function Declaration */

namespace swm::channel
{
    U64 GetChannelElemetSize(U8 channelFlags);

    types::Vector2F ReadVector2(const CompressedChannel& ch, utils::BitStreamReader& bsr);
    types::Vector3F ReadVector3(const CompressedChannel& ch, utils::BitStreamReader& bsr);

    types::Color4U ReadColor4U(const channel::CompressedChannel& ch, utils::BitStreamReader& bsr,
                               types::mappingFlags::MappingBits mappingBits);

    types::BoneIndex ReadBoneIndex(const channel::CompressedChannel& ch, utils::BitStreamReader& bsr);
    types::BoneWeights ReadBoneWeight(const channel::CompressedChannel& ch, utils::BitStreamReader& bsr);

} // namespace swm::channel

#endif
