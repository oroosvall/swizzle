
/* Include files */

#include "InternalStructs.hpp"

#include <utils/BitUtil.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace swm::types
{

    U64 GetSizeOfAttribute(compressFlags::CompressedChannelAttribute attribute)
    {
        U64 size = 0ull;
        typedef types::compressFlags::CompressedChannelAttribute attr;
        switch (attribute)
        {
        case attr::Position:
        case attr::Normal:
            size = sizeof(types::Vector3F);
            break;
        case attr::Uv:
            size = sizeof(types::Vector2F);
            break;
        case attr::Color:
            size = sizeof(types::Color4U);
            break;
        case attr::BoneIndex:
            size = sizeof(types::BoneIndex);
            break;
        case attr::BoneWeight:
            size = sizeof(types::BoneWeights);
            break;
        default:
            break;
        }
        return size;
    }

    Result GetChannelSize(U8 channelAttributes, U64& outSize)
    {
        auto attribs = GetAttributes(channelAttributes & 0x3F); // Filter the compress and unused flag
        Result res = Result::ErrToManyAttributes;
        if (attribs.size() == 1u)
        {
            outSize = GetSizeOfAttribute(attribs[0u]);
            res = Result::Success;
        }
        return res;
    }

    std::vector<compressFlags::CompressedChannelAttribute> GetAttributes(U8 channelAttributes)
    {
        std::vector<types::compressFlags::CompressedChannelAttribute> attribs;

        for (U8 i = 0u; i < 8u; ++i)
        {
            if (utils::IsBitSet(channelAttributes, i))
            {
                attribs.push_back(types::compressFlags::CompressedChannelAttribute(i));
            }
        }
        return attribs;
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */

