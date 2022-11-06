
/* Include files */

#include "ChannelCommon.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

namespace swm::channel
{
    template <typename T>
    static T GetValue(const U8* data, U64 numIndex, U64 index);
}

/* Static Variables */

/* Static Function Definition */

namespace swm::channel
{
    template <typename T>
    static T GetValue(const U8* data, U64 numIndex, U64 index)
    {
        T* tPtr = (T*)data;
        if (index >= numIndex)
            return T{}; // T default value
        else
            return tPtr[index];
    }
} // namespace swm::channel

/* Function Definition */

namespace swm::channel
{
    types::Vector2F ReadVector2(const channel::CompressedChannel& ch, utils::BitStreamReader& bsr)
    {
        U32 idx1, idx2;
        bsr.readBits(idx1, ch.mBitsPerIndex);
        bsr.readBits(idx2, ch.mBitsPerIndex);

        return types::Vector2F{GetValue<F32>(ch.mData.data(), ch.mElementCount, idx1),
                               GetValue<F32>(ch.mData.data(), ch.mElementCount, idx2)};
    }

    types::Vector3F ReadVector3(const channel::CompressedChannel& ch, utils::BitStreamReader& bsr)
    {
        U32 idx1, idx2, idx3;
        bsr.readBits(idx1, ch.mBitsPerIndex);
        bsr.readBits(idx2, ch.mBitsPerIndex);
        bsr.readBits(idx3, ch.mBitsPerIndex);

        return types::Vector3F{GetValue<F32>(ch.mData.data(), ch.mElementCount, idx1),
                               GetValue<F32>(ch.mData.data(), ch.mElementCount, idx2),
                               GetValue<F32>(ch.mData.data(), ch.mElementCount, idx3)};
    }

    types::Color4U ReadColor4U(const channel::CompressedChannel& ch, utils::BitStreamReader& bsr,
                               types::mappingFlags::MappingBits mappingBits)
    {
        if (mappingBits == types::mappingFlags::OneToOne)
        {
            U32 idx1, idx2, idx3, idx4;
            bsr.readBits(idx1, ch.mBitsPerIndex);
            bsr.readBits(idx2, ch.mBitsPerIndex);
            bsr.readBits(idx3, ch.mBitsPerIndex);
            bsr.readBits(idx4, ch.mBitsPerIndex);

            return types::Color4U{GetValue<U8>(ch.mData.data(), ch.mElementCount, idx1),
                                  GetValue<U8>(ch.mData.data(), ch.mElementCount, idx2),
                                  GetValue<U8>(ch.mData.data(), ch.mElementCount, idx3),
                                  GetValue<U8>(ch.mData.data(), ch.mElementCount, idx4)};
        }
        else if (mappingBits == types::mappingFlags::OneToTwo)
        {
            U32 idx1, idx2;
            bsr.readBits(idx1, ch.mBitsPerIndex);
            bsr.readBits(idx2, ch.mBitsPerIndex);

            U16 value1 = GetValue<U16>(ch.mData.data(), ch.mElementCount, idx1);
            U16 value2 = GetValue<U16>(ch.mData.data(), ch.mElementCount, idx2);

            return types::Color4U{GetValue<U8>((U8*)&value1, 2u, 0u), GetValue<U8>((U8*)&value1, 2u, 1u),
                                  GetValue<U8>((U8*)&value2, 2u, 0u), GetValue<U8>((U8*)&value2, 2u, 1u)};
        }
        else if (mappingBits == types::mappingFlags::OneToFour)
        {
            U32 idx1;
            bsr.readBits(idx1, ch.mBitsPerIndex);

            U32 value = GetValue<U32>(ch.mData.data(), ch.mElementCount, idx1);

            return types::Color4U{GetValue<U8>((U8*)&value, 4u, 0u), GetValue<U8>((U8*)&value, 4u, 1u),
                                  GetValue<U8>((U8*)&value, 4u, 2u), GetValue<U8>((U8*)&value, 4u, 3u)};
        }
        else
        {
            return types::Color4U{};
        }
    }

    types::BoneIndex ReadBoneIndex(const channel::CompressedChannel& ch, utils::BitStreamReader& bsr)
    {
        U32 idx1, idx2, idx3, idx4;
        bsr.readBits(idx1, ch.mBitsPerIndex);
        bsr.readBits(idx2, ch.mBitsPerIndex);
        bsr.readBits(idx3, ch.mBitsPerIndex);
        bsr.readBits(idx4, ch.mBitsPerIndex);

        return types::BoneIndex{GetValue<U16>(ch.mData.data(), ch.mElementCount, idx1),
                                GetValue<U16>(ch.mData.data(), ch.mElementCount, idx2),
                                GetValue<U16>(ch.mData.data(), ch.mElementCount, idx3),
                                GetValue<U16>(ch.mData.data(), ch.mElementCount, idx4)};
    }

    types::BoneWeights ReadBoneWeight(const channel::CompressedChannel& ch, utils::BitStreamReader& bsr)
    {
        U32 idx1, idx2, idx3, idx4;
        bsr.readBits(idx1, ch.mBitsPerIndex);
        bsr.readBits(idx2, ch.mBitsPerIndex);
        bsr.readBits(idx3, ch.mBitsPerIndex);
        bsr.readBits(idx4, ch.mBitsPerIndex);

        return types::BoneWeights{GetValue<F32>(ch.mData.data(), ch.mElementCount, idx1),
                                  GetValue<F32>(ch.mData.data(), ch.mElementCount, idx2),
                                  GetValue<F32>(ch.mData.data(), ch.mElementCount, idx3),
                                  GetValue<F32>(ch.mData.data(), ch.mElementCount, idx4)};
    }

} // namespace swm::channel

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
