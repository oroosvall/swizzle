
/* Include files */

#include "LoadV1_x.hpp"

#include "Ok.hpp"

#include <utils/BitUtil.hpp>
#include <utils/Bitstream.hpp>

#include <functional>

/* Defines */

/* Typedefs */

/* Structs/Classes */

namespace swm::load
{
    template <typename T>
    struct LoadFuncs
    {
        SwBool mCondition;
        std::function<T(U64)> mFunction;

        void execute(U64 count, T& out)
        {
            if (mCondition)
            {
                out = mFunction(count);
            }
        }
    };
} // namespace swm::load

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

namespace swm::load
{
    types::Vector2F ReadVector2(const channel::CompressedChannel& ch, utils::BitStreamReader& bsr);
    types::Vector3F ReadVector3(const channel::CompressedChannel& ch, utils::BitStreamReader& bsr);

    template <typename T>
    T GetValue(const U8* data, U64 maxIndex, U64 index)
    {
        T* tPtr = (T*)data;
        if (index > maxIndex)
            return {}; // T default value
        else
            return tPtr[index];
    }

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
            U16 value2 = GetValue<U16>(ch.mData.data(), ch.mElementCount, idx1);

            return types::Color4U{GetValue<U8>((U8*)&value1, 1u, 0u), GetValue<U8>((U8*)&value1, 1u, 1u),
                                  GetValue<U8>((U8*)&value2, 1u, 0u), GetValue<U8>((U8*)&value2, 1u, 1u)};
        }
        else if (mappingBits == types::mappingFlags::OneToFour)
        {
            U32 idx1;
            bsr.readBits(idx1, ch.mBitsPerIndex);

            U32 value = GetValue<U32>(ch.mData.data(), ch.mElementCount, idx1);

            return types::Color4U{GetValue<U8>((U8*)&value, 3u, 0u), GetValue<U8>((U8*)&value, 3u, 1u),
                                  GetValue<U8>((U8*)&value, 3u, 2u), GetValue<U8>((U8*)&value, 3u, 3u)};
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
} // namespace swm::load

/* Function Definition */

/* Class Public Function Definition */

namespace swm::load
{
    /*
     * MeshLoaderV1_x
     */
    MeshLoaderV1_x::MeshLoaderV1_x(LoaderCommonIfc& commonLdr, VTLoaderIfc& vtLoader, const types::Header& header)
        : mCommonLdr(commonLdr)
        , mVtLoader(vtLoader)
        , mHeader(header)
    {
    }

    Result MeshLoaderV1_x::loadMeshHeader(U32& num)
    {
        U8 count = 0u;
        Result res = mCommonLdr.loadNumber(count);
        num = count;
        return res;
    }

    Result MeshLoaderV1_x::loadMeshData(types::Mesh& mesh)
    {
        Result res = mCommonLdr.loadShortString(mesh.mName);
        res = Ok(res) ? mCommonLdr.loadNumber(mesh.mFlags) : res;
        if (utils::IsBitSet(mesh.mFlags, types::meshFlags::VERTEX_COMPRESS_BIT))
        {
            res = Ok(res) ? mVtLoader.loadVertexDataCompressed(mesh) : res;
        }
        else
        {
            res = Ok(res) ? mVtLoader.loadVertexData(mesh) : res;
        }

        if (utils::IsBitSet(mesh.mFlags, types::meshFlags::INDEX_COMPRESS_BIT))
        {
            res = Ok(res) ? mVtLoader.loadTriangleDataCompressed(mesh) : res;
        }
        else
        {
            res = Ok(res) ? mVtLoader.loadTriangleData(mesh) : res;
        }

        if (utils::IsBitSet(mesh.mFlags, types::meshFlags::ANIMATION_BIT))
        {
            if (utils::IsBitSet(mesh.mFlags, types::meshFlags::ANIMATION_COMPRESS_BIT))
                res = Ok(res) ? mVtLoader.loadAnimationDataCompressed(mesh) : res;
            else
                res = Ok(res) ? mVtLoader.loadAnimationData(mesh) : res;
        }

        return res;
    }

    /*
     * VTLoaderV1_x
     */

    VTLoaderV1_x::VTLoaderV1_x(LoaderCommonIfc& commonLdr, const types::Header& header)
        : mCommonLdr(commonLdr)
        , mHeader(header)
    {
    }

    Result VTLoaderV1_x::loadVertexData(types::Mesh& mesh)
    {
        U32 numVertices = 0u;
        Result res = mCommonLdr.loadNumber(numVertices);

        auto bc = [&](types::meshFlags::MeshFlags flags) { return utils::IsBitSet(mesh.mFlags, flags); };

        std::vector<types::BoneIndex> boneIndex;
        std::vector<types::BoneWeights> boneWeights;

        LoadFuncs<Result> loads[] = {
            {true, [&](U64 count) { return mCommonLdr.loadArray(mesh.mPositions, count); }},
            {bc(types::meshFlags::UV_BIT), [&](U64 count) { return mCommonLdr.loadArray(mesh.mUvs, count); }},
            {bc(types::meshFlags::NORMAL_BIT), [&](U64 count) { return mCommonLdr.loadArray(mesh.mNormals, count); }},
            {bc(types::meshFlags::COLOR_BIT), [&](U64 count) { return mCommonLdr.loadArray(mesh.mVertColors, count); }},
            {bc(types::meshFlags::ANIMATION_BIT), [&](U64 count) { return mCommonLdr.loadArray(boneIndex, count); }},
            {bc(types::meshFlags::ANIMATION_BIT), [&](U64 count) { return mCommonLdr.loadArray(boneWeights, count); }},
        };

        if (Ok(res))
        {
            for (auto& fn : loads)
            {
                fn.execute(static_cast<U64>(numVertices), res);
                if (res != Result::Success)
                {
                    break;
                }
            }
        }

        if (bc(types::meshFlags::ANIMATION_BIT) && res == Result::Success)
        {
            mesh.mBoneInfo.resize(boneIndex.size());
            for (U64 j = 0u; j < boneIndex.size(); ++j)
            {
                mesh.mBoneInfo[j].mIndex = boneIndex[j];
                mesh.mBoneInfo[j].mWeights = boneWeights[j];
            };
        }

        return res;
    }

    Result VTLoaderV1_x::loadTriangleData(types::Mesh& mesh)
    {
        U32 triangleCount = 0u;
        Result res = mCommonLdr.loadNumber(triangleCount);
        res = Ok(res) ? mCommonLdr.loadArray(mesh.mTriangles, triangleCount) : res;
        return res;
    }

    Result VTLoaderV1_x::loadAnimationData(types::Mesh& mesh)
    {
        types::AnimationInfo& ai = mesh.mAnimationData;

        Result res = mCommonLdr.loadNumber(ai.mFramerate);
        res = Ok(res) ? mCommonLdr.loadNumber(ai.mNumBones) : res;
        res = Ok(res) ? mCommonLdr.loadArray(ai.mParentList, ai.mNumBones) : res;
        res = Ok(res) ? mCommonLdr.loadArray(ai.mBindPose, ai.mNumBones) : res;

        U16 animCount = 0u;
        res = Ok(res) ? mCommonLdr.loadNumber(animCount) : res;

        for (U16 i = 0u; (i < animCount) && Ok(res); ++i)
        {
            types::Animation anim;
            res = Ok(res) ? mCommonLdr.loadShortString(anim.mName) : res;
            U16 keyframeCount = 0u;
            res = Ok(res) ? mCommonLdr.loadNumber(keyframeCount) : res;

            anim.mKeyFrames.resize(keyframeCount);
            for (auto& frame : anim.mKeyFrames)
            {
                res = Ok(res) ? mCommonLdr.loadArray(frame.mFrameData, ai.mNumBones) : res;
                if (res != Result::Success)
                    break;
            }
            ai.mAnimations.emplace_back(anim);
        }

        return res;
    }

    Result VTLoaderV1_x::loadVertexDataCompressed(types::Mesh& mesh)
    {
        U8 compressFlags = 0u;
        U8 numberOfChannles = 0u;

        Result res = mCommonLdr.loadNumber(compressFlags);
        res = Ok(res) ? mCommonLdr.loadNumber(numberOfChannles) : res;

        std::vector<channel::CompressedChannel> channels;
        for (U32 i = 0u; i < numberOfChannles && Ok(res); ++i)
        {
            channel::CompressedChannel ch = {};
            res = loadChannel(ch, utils::IsBitSet(mesh.mFlags, types::meshFlags::ANIMATION_BIT));
            channels.emplace_back(ch);
        }

        for (auto& ch : channels)
        {
            processChannel(ch, mesh);
        }

        return res;
    }

    Result VTLoaderV1_x::loadTriangleDataCompressed(types::Mesh& mesh)
    {
        U8 compressFlags = 0u;
        U8 bitsPerVertex = 0u;
        U32 numTriangles = 0u;

        Result res = mCommonLdr.loadNumber(compressFlags);
        res = Ok(res) ? mCommonLdr.loadNumber(bitsPerVertex) : res;
        res = Ok(res) ? mCommonLdr.loadNumber(numTriangles) : res;

        size_t size = ((numTriangles * 3ull * bitsPerVertex) + 7ull) / 8ull;
        std::vector<U8> data;
        res = Ok(res) ? mCommonLdr.loadArray(data, size) : res;

        if (Ok(res))
        {
            utils::BitStreamReader bsr(data.data(), size);

            for (size_t j = 0ull; j < numTriangles; j++)
            {
                U32 v1, v2, v3;
                bsr.readBits(v1, bitsPerVertex);
                bsr.readBits(v2, bitsPerVertex);
                bsr.readBits(v3, bitsPerVertex);

                mesh.mTriangles.push_back({v1, v2, v3});
            }
        }

        return res;
    }

    Result VTLoaderV1_x::loadAnimationDataCompressed(types::Mesh& mesh)
    {
        types::AnimationInfo& ai = mesh.mAnimationData;

        Result res = mCommonLdr.loadNumber(ai.mFramerate);
        res = Ok(res) ? mCommonLdr.loadNumber(ai.mNumBones) : res;
        res = Ok(res) ? mCommonLdr.loadArray(ai.mParentList, ai.mNumBones) : res;
        res = Ok(res) ? mCommonLdr.loadArray(ai.mBindPose, ai.mNumBones) : res;

        U16 animCount = 0u;
        res = Ok(res) ? mCommonLdr.loadNumber(animCount) : res;

        U32 elementCount = 0u;
        res = Ok(res) ? mCommonLdr.loadNumber(elementCount) : res;
        std::vector<F32> elements;
        res = Ok(res) ? mCommonLdr.loadArray(elements, elementCount) : res;

        U8 bitCount = utils::bits_needed(elementCount);

        for (U16 i = 0u; (i < animCount) && Ok(res); ++i)
        {
            types::Animation anim;
            res = Ok(res) ? mCommonLdr.loadShortString(anim.mName) : res;
            U16 keyframeCount = 0u;
            res = Ok(res) ? mCommonLdr.loadNumber(keyframeCount) : res;

            anim.mKeyFrames.resize(keyframeCount);

            for (auto& frame : anim.mKeyFrames)
            {
                U64 size = ((ai.mNumBones * 16ull * bitCount) + 7ull) / 8ull;
                std::vector<U8> data;
                res = Ok(res) ? mCommonLdr.loadArray(data, size) : res;
                if (Ok(res))
                {
                    utils::BitStreamReader bsr(data.data(), size);
                    frame.mFrameData.resize(ai.mNumBones);
                    for (auto& mat : frame.mFrameData)
                    {
                        for (auto& f : mat.mMat)
                        {
                            U32 idx = 0ul;
                            bsr.readBits(idx, bitCount);
                            f = elements[idx];
                        }
                    }
                }
            }

            ai.mAnimations.emplace_back(anim);
        }

        return res;
    }
} // namespace swm::load

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace swm::load
{
    Result VTLoaderV1_x::loadChannel(channel::CompressedChannel& ch, SwBool hasAnimations)
    {
        Result r = mCommonLdr.loadNumber(ch.mChannelAttribute);
        const SwBool compressedChannel =
            utils::IsBitSet(ch.mChannelAttribute, (U8)types::compressFlags::CompressedChannelAttribute::Compressed);

        if (compressedChannel)
        {
            auto attribs = types::GetAttributes(ch.mChannelAttribute & 0x3F);
            U64 attribCount = 0u;
            for (auto& attr : attribs)
            {
                switch (attr)
                {
                case types::compressFlags::CompressedChannelAttribute::Position:
                case types::compressFlags::CompressedChannelAttribute::Normal:
                    attribCount += 3ull;
                    break;
                case types::compressFlags::CompressedChannelAttribute::Uv:
                    attribCount += 2ull;
                    break;
                case types::compressFlags::CompressedChannelAttribute::Color:
                case types::compressFlags::CompressedChannelAttribute::BoneIndex:
                case types::compressFlags::CompressedChannelAttribute::BoneWeight:
                    attribCount += 4ull;
                    break;
                default:
                    break;
                }
            }
            r = Ok(r) ? mCommonLdr.loadNumber(ch.mChannelDataFlags) : r;
            r = Ok(r) ? mCommonLdr.loadNumber(ch.mElementCount) : r;
            U64 size = ch.mElementCount * channel::GetChannelElemetSize(ch.mChannelDataFlags);
            r = Ok(r) ? mCommonLdr.loadArray(ch.mData, size) : r;
            r = Ok(r) ? mCommonLdr.loadNumber(ch.mBitsPerIndex) : r;
            r = Ok(r) ? mCommonLdr.loadNumber(ch.mVertexCount) : r;
            size = ((static_cast<U64>(ch.mBitsPerIndex) * attribCount * ch.mVertexCount) + 7ull) / 8ull;
            r = Ok(r) ? mCommonLdr.loadArray(ch.mBitData, size) : r;
        }
        else
        {
            r = Ok(r) ? mCommonLdr.loadNumber(ch.mVertexCount) : r;
            U64 channelSize = 0ull;
            r = Ok(r) ? types::GetChannelSize(ch.mChannelAttribute, channelSize) : r;
            U64 size = ch.mVertexCount * channelSize;
            r = Ok(r) ? mCommonLdr.loadArray(ch.mData, size) : r;
        }

        if (!hasAnimations)
        {
            // This will make sure that boneIndex and BoneWeight are not set
            ch.mChannelAttribute &= 0xCF;
        }
        return r;
    }

    Result VTLoaderV1_x::processChannel(const channel::CompressedChannel& ch, types::Mesh& mesh)
    {
        // Only get channel attributes of interest
        std::vector<types::compressFlags::CompressedChannelAttribute> attribs =
            types::GetAttributes(ch.mChannelAttribute & 0x3F);
        const SwBool compressedChannel =
            utils::IsBitSet(ch.mChannelAttribute, (U8)types::compressFlags::CompressedChannelAttribute::Compressed);

        if (compressedChannel)
        {
            utils::BitStreamReader bsr(ch.mBitData.data(), ch.mBitData.size());

            std::vector<types::BoneIndex> boneIndex;
            std::vector<types::BoneWeights> boneWeights;

            std::function<void()> fn[] = {
                [&] { mesh.mPositions.emplace_back(ReadVector3(ch, bsr)); },
                [&] { mesh.mUvs.emplace_back(ReadVector2(ch, bsr)); },
                [&] { mesh.mNormals.emplace_back(ReadVector3(ch, bsr)); },
                [&] {
                    mesh.mVertColors.emplace_back(ReadColor4U(ch, bsr, getChannelMappingBits(ch.mChannelDataFlags)));
                },
                [&] { boneIndex.emplace_back(ReadBoneIndex(ch, bsr)); },
                [&] { boneWeights.emplace_back(ReadBoneWeight(ch, bsr)); },
            };

            SwBool hasAnim = false;
            for (U64 i = 0ull; i < ch.mVertexCount; ++i)
            {
                for (const auto& a : attribs)
                {
                    fn[U8(a)]();
                    if ((a == types::compressFlags::CompressedChannelAttribute::BoneIndex) ||
                        (a == types::compressFlags::CompressedChannelAttribute::BoneWeight))
                    {
                        hasAnim = true;
                    }
                }
            }

            if (hasAnim)
            {
                mesh.mBoneInfo.resize(ch.mVertexCount);
                for (U64 i = 0ull; i < boneIndex.size(); ++i)
                {
                    mesh.mBoneInfo[i].mIndex = boneIndex[i];
                }
                for (U64 i = 0ull; i < boneWeights.size(); ++i)
                {
                    mesh.mBoneInfo[i].mWeights = boneWeights[i];
                }
            }
        }
        else
        {
            std::vector<types::BoneIndex> boneIndex;
            std::vector<types::BoneWeights> boneWeights;

            std::function<void()> fn[] = {
                [&] {
                    mesh.mPositions.resize(ch.mVertexCount);
                    memcpy(mesh.mPositions.data(), ch.mData.data(), ch.mVertexCount * sizeof(types::Vector3F));
                },
                [&] {
                    mesh.mUvs.resize(ch.mVertexCount);
                    memcpy(mesh.mUvs.data(), ch.mData.data(), ch.mVertexCount * sizeof(types::Vector2F));
                },
                [&] {
                    mesh.mNormals.resize(ch.mVertexCount);
                    memcpy(mesh.mNormals.data(), ch.mData.data(), ch.mVertexCount * sizeof(types::Vector3F));
                },
                [&] {
                    mesh.mVertColors.resize(ch.mVertexCount);
                    memcpy(mesh.mVertColors.data(), ch.mData.data(), ch.mVertexCount * sizeof(types::Color4U));
                },
                [&] {
                    boneIndex.resize(ch.mVertexCount);
                    memcpy(boneIndex.data(), ch.mData.data(), ch.mVertexCount * sizeof(types::BoneIndex));
                },
                [&] {
                    boneWeights.resize(ch.mVertexCount);
                    memcpy(boneWeights.data(), ch.mData.data(), ch.mVertexCount * sizeof(types::BoneIndex));
                }};

            for (const auto& a : attribs)
            {
                fn[U8(a)]();
                if (a == types::compressFlags::CompressedChannelAttribute::BoneIndex)
                {
                    mesh.mBoneInfo.resize(ch.mVertexCount);
                    for (U64 i = 0ull; i < ch.mVertexCount; ++i)
                    {
                        mesh.mBoneInfo[i].mIndex = boneIndex[i];
                    }
                }
                else if (a == types::compressFlags::CompressedChannelAttribute::BoneWeight)
                {
                    mesh.mBoneInfo.resize(ch.mVertexCount);
                    for (U64 i = 0ull; i < ch.mVertexCount; ++i)
                    {
                        mesh.mBoneInfo[i].mWeights = boneWeights[i];
                    }
                }
                else
                {
                    // Do nothing
                }
            }
        }

        return Result::Success;
    }

    types::mappingFlags::MappingBits VTLoaderV1_x::getChannelMappingBits(U8 channelFlags)
    {
        return types::mappingFlags::MappingBits((channelFlags & 0xC0) >> 6);
    }

} // namespace swm::load
