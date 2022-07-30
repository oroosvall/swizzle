#ifndef INTERNAL_STRUCTS_HPP
#define INTERNAL_STRUCTS_HPP

/* Include files */

#include <swm/Swm.hpp>
#include <common/Common_new.hpp>

/* Defines */

/* Typedefs/enums */

namespace swm::types::meshFlags
{
    enum MeshFlags
    {
        UV_BIT = 0x00,
        NORMAL_BIT = 0x01,
        COLOR_BIT = 0x02,
        ANIMATION_BIT = 0x08,
        ANIMATION_COMPRESS_BIT = 0x0D,
        VERTEX_COMPRESS_BIT = 0x0E,
        INDEX_COMPRESS_BIT = 0x0F
    };
}

namespace swm::types::compressFlags
{
    enum class CompressedChannelAttribute
    {
        Position = 0,
        Uv = 1,
        Normal = 2,
        Color = 3,
        BoneIndex = 4,
        BoneWeight = 5,
        Unused = 6,
        Compressed = 7,
    };
}

namespace swm::types::mappingFlags
{
    enum MappingBits
    {
        OneToOne = 0,
        OneToTwo = 1,
        OneToFour = 2,
    };
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swm::types
{
#pragma pack(push, 1)

    struct Version
    {
        U16 mMajor;
        U16 mMinor;
    };

    struct Header
    {
        U8 mMagic[4];
        Version mVersion;
    };

#pragma pack(pop)
}

/* Class Declaration */

/* Function Declaration */

namespace swm::types
{
    Result GetAttributeSize(U8 channelAttributes, U64& outSize);
    std::vector<compressFlags::CompressedChannelAttribute> GetAttributes(U8 channelAttributes);
}

#endif
