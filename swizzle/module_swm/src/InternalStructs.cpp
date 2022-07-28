
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
    Result GetAttributeSize(U8 channelAttributes, U64& outSize)
    {
        auto attribs = GetAttributes(channelAttributes & 0x3F); // Filter the compress and unused flag
        Result res = Result::ErrToManyAttributes;
        typedef types::compressFlags::CompressedChannelAttribute attr;
        if (attribs.size() == 1)
        {
            switch (attribs[0])
            {
            case attr::Position:
            case attr::Normal:
                outSize = sizeof(types::Vector3F);
                break;
            case attr::Uv:
                outSize = sizeof(types::Vector2F);
                break;
            case attr::Color:
                outSize = sizeof(types::Color4U);
                break;
            case attr::BoneIndex:
                outSize = sizeof(types::BoneIndex);
                break;
            case attr::BoneWeight:
                outSize = sizeof(types::BoneWeights);
                break;
            default:
                break;
            }
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

