
/* Include files */

#include "SaveV1_x.hpp"
#include "Ok.hpp"

#include <utils/BitUtil.hpp>
#include <utils/Bitstream.hpp>

#include <set>

/* Defines */

/* Typedefs */

/* Structs/Classes */

namespace swm::save
{
    enum class Type
    {
        tU8,
        tU16,
        tU32,
        tF32
    };

    struct ISet
    {
        virtual ~ISet() = default;
        virtual Type getType() = 0;
    };

    template<typename T>
    struct TSet : public ISet
    {
        virtual ~TSet() = default;
        virtual Type getType() override { return mType; }

        std::set<T> mSet;
        Type mType;
        U8 mAttribute;
        U32 mVertexCount;
    };
}

/* Static Function Declaration */

namespace swm::save
{
    RawChannel setupChannel(U8* data, U64 count, U8 elemetSize, U8 componentCount, types::compressFlags::CompressedChannelAttribute attrib);
}

/* Static Variables */

/* Static Function Definition */

namespace swm::save
{
    RawChannel setupChannel(U8* data, U64 count, U8 elemetSize, U8 componentCount, types::compressFlags::CompressedChannelAttribute attrib)
    {
        RawChannel ch {};
        ch.mData = new U8[count * elemetSize];
        memcpy(ch.mData, data, count * elemetSize);
        ch.mElementCount = count;
        ch.mElementSize = elemetSize;
        ch.mComponentCount = componentCount;
        ch.mAttrib = attrib;
        return ch;
    }
}

/* Function Definition */

/* Class Public Function Definition */

namespace swm::save
{
    MeshSaverV1_x::MeshSaverV1_x(SaverCommonIfc& commonLdr, VTSaverIfc& vtSaver, const types::Header& header)
        : mCommonLdr(commonLdr)
        , mVtSaver(vtSaver)
        , mHeader(header)
    {
    }

    Result MeshSaverV1_x::saveMeshHeader(const U32& num)
    {
        U8 count = static_cast<U8>(num);
        return mCommonLdr.saveNumber(count);
    }

    Result MeshSaverV1_x::saveMeshData(const types::Mesh& mesh, const options::SaveOptions& options)
    {
        auto getFlags = [&]() -> U16 {
            U16 f = 0u;
            if (!mesh.mUvs.empty()) { utils::SetBit(f, types::meshFlags::UV_BIT); }
            if (!mesh.mNormals.empty()) { utils::SetBit(f, types::meshFlags::NORMAL_BIT); }
            if (!mesh.mVertColors.empty()) { utils::SetBit(f, types::meshFlags::COLOR_BIT); }
            if (!mesh.mBoneInfo.empty()) { utils::SetBit(f, types::meshFlags::ANIMATION_BIT); }
            if (options.mCompressVertex) {utils::SetBit(f, types::meshFlags::VERTEX_COMPRESS_BIT); }
            if (options.mCompressIndex) {utils::SetBit(f, types::meshFlags::INDEX_COMPRESS_BIT); }
            return f;
        };

        U16 flags = getFlags();
        Result res = mCommonLdr.saveShortString(mesh.mName);
        res = Ok(res) ? mCommonLdr.saveNumber(flags) : res;
        if (options.mCompressVertex)
            res = Ok(res) ? mVtSaver.saveVertexDataCompressed(mesh, flags) : res;
        else
            res = Ok(res) ? mVtSaver.saveVertexData(mesh, flags) : res;

        if (options.mCompressIndex)
            res = Ok(res) ? mVtSaver.saveTriangleDataCompressed(mesh) : res;
        else
            res = Ok(res) ? mVtSaver.saveTriangleData(mesh, flags) : res;

        if (utils::IsBitSet(mesh.mFlags, types::meshFlags::ANIMATION_BIT))
        {
            res = Ok(res) ? mVtSaver.saveAnimationData(mesh) : res;
        }

        return res;
    }

    /*
    * VTSaverV1_x
    */

    VTSaverV1_x::VTSaverV1_x(SaverCommonIfc& commonLdr, const types::Header& header)
        : mCommonLdr(commonLdr)
        , mHeader(header)
    {
    }

    Result VTSaverV1_x::saveVertexData(const types::Mesh& mesh, const U16 flags)
    {
        size_t vertexCount = mesh.mPositions.size();

        auto check = [&]() -> bool {
            bool ok = true;
            ok = utils::IsBitSet(flags, types::meshFlags::UV_BIT) ? (mesh.mUvs.size() == vertexCount) : ok;
            ok = utils::IsBitSet(flags, types::meshFlags::NORMAL_BIT) ? (mesh.mNormals.size() == vertexCount) : ok;
            ok = utils::IsBitSet(flags, types::meshFlags::COLOR_BIT) ? (mesh.mVertColors.size() == vertexCount) : ok;
            ok = utils::IsBitSet(flags, types::meshFlags::ANIMATION_BIT) ? (mesh.mBoneInfo.size() == vertexCount) : ok;
            return ok;
        };
        Result res = check() ? Result::Success : Result::ErrDataSizeMissmatch;

        std::vector<types::BoneIndex> boneIndex;
        std::vector<types::BoneWeights> boneWeights;

        if (utils::IsBitSet(flags, types::meshFlags::ANIMATION_BIT))
        {
            boneIndex.resize(mesh.mBoneInfo.size());
            boneWeights.resize(mesh.mBoneInfo.size());

            for (U64 j = 0u; j < boneIndex.size(); ++j)
            {
                boneIndex[j] = mesh.mBoneInfo[j].mIndex;
                boneWeights[j] = mesh.mBoneInfo[j].mWeights;
            };
        }

        res = Ok(res) ? mCommonLdr.saveNumber(static_cast<U32>(vertexCount)) : res;

        auto store = [&]() -> Result {
            Result r = mCommonLdr.saveArray(mesh.mPositions);
            r = utils::IsBitSet(flags, types::meshFlags::UV_BIT) ? mCommonLdr.saveArray(mesh.mUvs) : r;
            r = utils::IsBitSet(flags, types::meshFlags::NORMAL_BIT) ? mCommonLdr.saveArray(mesh.mNormals) : r;
            r = utils::IsBitSet(flags, types::meshFlags::COLOR_BIT) ? mCommonLdr.saveArray(mesh.mVertColors) : r;
            r = utils::IsBitSet(flags, types::meshFlags::ANIMATION_BIT) ? mCommonLdr.saveArray(boneIndex) : r;
            r = utils::IsBitSet(flags, types::meshFlags::ANIMATION_BIT) ? mCommonLdr.saveArray(boneWeights) : r;
            return r;
        };

        res = Ok(res) ? store() : res;

        return res;
    }

    Result VTSaverV1_x::saveTriangleData(const types::Mesh& mesh, const U16 flags)
    {
        UNUSED_ARG(flags);

        U32 triangleCount = static_cast<U32>(mesh.mTriangles.size());
        Result res = mCommonLdr.saveNumber(triangleCount);
        res = Ok(res) ? mCommonLdr.saveArray(mesh.mTriangles) : res;
        return res;
    }

    Result VTSaverV1_x::saveAnimationData(const types::Mesh& mesh)
    {
        const types::AnimationInfo& ai = mesh.mAnimationData;

        Result res = mCommonLdr.saveNumber(ai.mFramerate);
        res = Ok(res) ? mCommonLdr.saveNumber(ai.mNumBones) : res;
        res = Ok(res) ? mCommonLdr.saveArray(ai.mParentList) : res;
        res = Ok(res) ? mCommonLdr.saveArray(ai.mBindPose) : res;

        U16 animCount = static_cast<U16>(ai.mAnimations.size());
        res = Ok(res) ? mCommonLdr.saveNumber(animCount) : res;

        for (U16 i = 0u; (i < animCount) && Ok(res); ++i)
        {
            const types::Animation& anim = ai.mAnimations[i];
            res = Ok(res) ? mCommonLdr.saveShortString(anim.mName) : res;
            U16 keyframeCount = static_cast<U16>(anim.mKeyFrames.size());
            res = Ok(res) ? mCommonLdr.saveNumber(keyframeCount) : res;

            for (auto& frame : anim.mKeyFrames)
            {
                res = Ok(res) ? mCommonLdr.saveArray(frame.mFrameData) : res;
                if (res != Result::Success) break;
            }
        }

        return res;
    }

    Result VTSaverV1_x::saveVertexDataCompressed(const types::Mesh& mesh, const U16 flags)
    {
        std::vector<RawChannel> rawChannels;
        std::vector<CompressedChannel> compChannels;

        Result res = createChannels(rawChannels, mesh, flags);
        res = Ok(res) ? buildChannels(rawChannels, compChannels) : res;
        res = Ok(res) ? saveChannels(compChannels) : res;

        for (auto rch : rawChannels)
        {
            delete[] rch.mData;
        }

        return res;
    }

    Result VTSaverV1_x::saveTriangleDataCompressed(const types::Mesh& mesh)
    {
        U8 compressFlags = 0u;
        U32 numVertices = static_cast<U32>(mesh.mPositions.size());
        U32 numTriangles = static_cast<U32>(mesh.mTriangles.size());

        U8 bitsPerVertex = utils::bits_needed(numVertices - 1);

        const size_t size = (((size_t)numTriangles * (size_t)bitsPerVertex * 3U) + 7U) / 8U;
        U8* data = new uint8_t[size];

        utils::BitStreamWriter bsw(data, size);

        for (const auto& t : mesh.mTriangles)
        {
            bsw.writeBits(t.v1, bitsPerVertex);
            bsw.writeBits(t.v2, bitsPerVertex);
            bsw.writeBits(t.v3, bitsPerVertex);
        }

        bsw.flush();

        // All data is computed write it to file now
        Result res = mCommonLdr.saveNumber(compressFlags);
        res = Ok(res) ? mCommonLdr.saveNumber(bitsPerVertex) : res;
        res = Ok(res) ? mCommonLdr.saveNumber(numTriangles) : res;
        res = Ok(res) ? mCommonLdr.saveArray(data, bsw.getWrittenSize()) : res;

        return res;
    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace swm::save
{
    Result VTSaverV1_x::createChannels(std::vector<RawChannel>& channels, const types::Mesh& mesh, const U16 flags)
    {
        size_t vertexCount = mesh.mPositions.size();

        auto check = [&]() -> bool {
            bool ok = true;
            ok = utils::IsBitSet(flags, types::meshFlags::UV_BIT) ? (mesh.mUvs.size() == vertexCount) : ok;
            ok = utils::IsBitSet(flags, types::meshFlags::NORMAL_BIT) ? (mesh.mNormals.size() == vertexCount) : ok;
            ok = utils::IsBitSet(flags, types::meshFlags::COLOR_BIT) ? (mesh.mVertColors.size() == vertexCount) : ok;
            ok = utils::IsBitSet(flags, types::meshFlags::ANIMATION_BIT) ? (mesh.mBoneInfo.size() == vertexCount) : ok;
            return ok;
        };

        Result res = check() ? Result::Success : Result::ErrDataSizeMissmatch;

        std::vector<types::BoneIndex> boneIndex;
        std::vector<types::BoneWeights> boneWeights;

        if (utils::IsBitSet(flags, types::meshFlags::ANIMATION_BIT))
        {
            boneIndex.resize(mesh.mBoneInfo.size());
            boneWeights.resize(mesh.mBoneInfo.size());

            for (U64 j = 0u; j < boneIndex.size(); ++j)
            {
                boneIndex[j] = mesh.mBoneInfo[j].mIndex;
                boneWeights[j] = mesh.mBoneInfo[j].mWeights;
            };
        }

        auto initChannels = [&]() {
            typedef types::compressFlags::CompressedChannelAttribute Attrib;
            channels.push_back(setupChannel((U8*)mesh.mPositions.data(), mesh.mPositions.size() * 3u, sizeof(F32), 3u, Attrib::Position));
            if (utils::IsBitSet(flags, types::meshFlags::UV_BIT))
                { channels.push_back(setupChannel((U8*)mesh.mUvs.data(), mesh.mUvs.size() * 2u, sizeof(F32), 2u, Attrib::Uv)); }
            if (utils::IsBitSet(flags, types::meshFlags::NORMAL_BIT))
                { channels.push_back(setupChannel((U8*)mesh.mNormals.data(), mesh.mNormals.size() * 3u, sizeof(F32), 3u, Attrib::Normal)); }
            if (utils::IsBitSet(flags, types::meshFlags::COLOR_BIT))
                { channels.push_back(setupChannel((U8*)mesh.mVertColors.data(), mesh.mVertColors.size() * 4u, sizeof(U8), 4u, Attrib::Color)); }
            if (utils::IsBitSet(flags, types::meshFlags::ANIMATION_BIT))
                { channels.push_back(setupChannel((U8*)boneIndex.data(), boneIndex.size() * 4u, sizeof(U16), 4u, Attrib::BoneIndex)); }
            if (utils::IsBitSet(flags, types::meshFlags::ANIMATION_BIT))
                { channels.push_back(setupChannel((U8*)boneWeights.data(), boneWeights.size() * 4u, sizeof(F32), 4u, Attrib::BoneWeight)); }
        };

        initChannels();

        return res;
    }

    Result VTSaverV1_x::buildChannels(const std::vector<RawChannel>& rawChannels, std::vector<CompressedChannel>& compressedChannels)
    {
        typedef types::compressFlags::CompressedChannelAttribute Attrib;

        std::vector<ISet*> sets;

        for (const auto& rch : rawChannels)
        {
            switch (rch.mAttrib)
            {
            case Attrib::Position:
            case Attrib::Uv:
            case Attrib::Normal:
            case Attrib::BoneWeight:
            {
                TSet<F32>* ts = new TSet<F32>();
                utils::SetBit(ts->mAttribute, static_cast<U8>(rch.mAttrib));
                ts->mType = Type::tF32;
                ts->mVertexCount = U32(rch.mElementCount / U64(rch.mComponentCount));

                F32* data = (F32*)rch.mData;
                for (size_t i = 0u; i < rch.mElementCount; i++)
                {
                    ts->mSet.insert(data[i]);
                }
                sets.emplace_back(ts);

                break;
            }
            case Attrib::Color:
            {
                TSet<U8>* ts = new TSet<U8>();
                utils::SetBit(ts->mAttribute, static_cast<U8>(rch.mAttrib));
                ts->mType = Type::tU8;
                ts->mVertexCount = U32(rch.mElementCount / U64(rch.mComponentCount));

                U8* data = rch.mData;
                for (size_t i = 0u; i < rch.mElementCount; i++)
                {
                    ts->mSet.insert(data[i]);
                }
                sets.emplace_back(ts);

                break;
            }
            case Attrib::BoneIndex:
            {
                TSet<U16>* ts = new TSet<U16>();
                utils::SetBit(ts->mAttribute, static_cast<U8>(rch.mAttrib));
                ts->mType = Type::tU16;
                ts->mVertexCount = U32(rch.mElementCount / U64(rch.mComponentCount));

                U16* data = (U16*)rch.mData;
                for (size_t i = 0u; i < rch.mElementCount; i++)
                {
                    ts->mSet.insert(data[i]);
                }
                sets.emplace_back(ts);

                break;
            }
            default:
                break;
            }
        }

        // Attempt merging channels
        // Todo


        auto getElemSize = [&](U8 size) {
            switch (size)
            {
            case 1:
                return 0;
            case 2:
                return 1;
            case 4:
                return 2;
            case 8:
                return 3;
            default:
                break;
            }
            return 0;
        };

        // Build final channels
        for (const auto& s : sets)
        {
            CompressedChannel cc{};
            switch (s->getType())
            {
            case Type::tF32:
            {
                TSet<F32>* ts = (TSet<F32>*)s;
                std::vector<F32> v = std::vector<F32>(ts->mSet.begin(), ts->mSet.end());
                cc.mElementCount = U32(v.size());
                cc.mData.resize(v.size() * sizeof(F32));
                memcpy(cc.mData.data(), v.data(), v.size() * sizeof(F32));
                cc.mBitsPerIndex = utils::bits_needed(cc.mElementCount - 1u);
                cc.mVertexCount = ts->mVertexCount;
                cc.mChannelAttribute = ts->mAttribute;

                std::vector<const RawChannel*> rChans;
                U64 attributeCount = 0u;
                auto attribs = types::GetAttributes(ts->mAttribute);
                for (const auto& rch : rawChannels)
                {
                    for (const auto& attr : attribs)
                    {
                        if (rch.mAttrib == attr)
                        {
                            rChans.push_back(&rch);
                            attributeCount += rch.mComponentCount;
                            cc.mChannelDataFlags = (U8)getElemSize(rch.mElementSize);
                            break;
                        }
                    }
                }
                U64 size = ((cc.mVertexCount * attributeCount * U64(cc.mBitsPerIndex)) * 7ull) / 8ull;

                U8* bitData = new U8[size];
                utils::BitStreamWriter bsw(bitData, size);

                if (!rChans.empty())
                {
                    for (size_t i = 0u; i < cc.mVertexCount; i++)
                    {
                        for (const auto rch : rChans)
                        {
                            size_t off = i * rch->mComponentCount;
                            for (size_t j = 0u; j < rch->mComponentCount; j++)
                            {
                                F32* fltData = (F32*)rch->mData;
                                auto it = std::lower_bound(v.begin(), v.end(), fltData[off + j]);
                                U32 idx = (U32)(it - v.begin());
                                bsw.writeBits(idx, cc.mBitsPerIndex);
                            }
                        }
                    }
                }

                bsw.flush();
                cc.mBitData.resize(bsw.getWrittenSize());
                memcpy(cc.mBitData.data(), bitData, bsw.getWrittenSize());
                utils::SetBit(cc.mChannelAttribute, static_cast<U8>(Attrib::Compressed));
                delete[] bitData;

                break;
            }
            case Type::tU16:
            {
                TSet<U16>* ts = (TSet<U16>*)s;
                std::vector<U16> v = std::vector<U16>(ts->mSet.begin(), ts->mSet.end());
                cc.mElementCount = U32(v.size());
                cc.mData.resize(v.size() * sizeof(U16));
                memcpy(cc.mData.data(), v.data(), v.size() * sizeof(U16));
                cc.mBitsPerIndex = utils::bits_needed(cc.mElementCount - 1u);
                cc.mVertexCount = ts->mVertexCount;
                cc.mChannelAttribute = ts->mAttribute;

                std::vector<const RawChannel*> rChans;
                U64 attributeCount = 0u;
                auto attribs = types::GetAttributes(ts->mAttribute);
                for (const auto& rch : rawChannels)
                {
                    for (const auto& attr : attribs)
                    {
                        if (rch.mAttrib == attr)
                        {
                            rChans.push_back(&rch);
                            attributeCount += rch.mComponentCount;
                            cc.mChannelDataFlags = (U8)getElemSize(rch.mElementSize);
                            break;
                        }
                    }
                }
                U64 size = ((cc.mVertexCount * attributeCount * U64(cc.mBitsPerIndex)) * 7ull) / 8ull;

                U8* bitData = new U8[size];
                utils::BitStreamWriter bsw(bitData, size);

                if (!rChans.empty())
                {
                    for (size_t i = 0u; i < cc.mVertexCount; i++)
                    {
                        for (const auto rch : rChans)
                        {
                            size_t off = i * rch->mComponentCount;
                            for (size_t j = 0u; j < rch->mComponentCount; j++)
                            {
                                U16* fltData = (U16*)rch->mData;
                                auto it = std::lower_bound(v.begin(), v.end(), fltData[off + j]);
                                U32 idx = (U32)(it - v.begin());
                                bsw.writeBits(idx, cc.mBitsPerIndex);
                            }
                        }
                    }
                }

                bsw.flush();
                cc.mBitData.resize(bsw.getWrittenSize());
                memcpy(cc.mBitData.data(), bitData, bsw.getWrittenSize());
                utils::SetBit(cc.mChannelAttribute, static_cast<U8>(Attrib::Compressed));
                delete[] bitData;

                break;
            }
            case Type::tU32:
            {
                TSet<U32>* ts = (TSet<U32>*)s;
                std::vector<U32> v = std::vector<U32>(ts->mSet.begin(), ts->mSet.end());
                cc.mElementCount = U32(v.size());
                cc.mData.resize(v.size() * sizeof(U32));
                memcpy(cc.mData.data(), v.data(), v.size() * sizeof(U32));
                cc.mBitsPerIndex = utils::bits_needed(cc.mElementCount - 1u);
                cc.mVertexCount = ts->mVertexCount;
                cc.mChannelAttribute = ts->mAttribute;

                std::vector<const RawChannel*> rChans;
                U64 attributeCount = 0u;
                auto attribs = types::GetAttributes(ts->mAttribute);
                for (const auto& rch : rawChannels)
                {
                    for (const auto& attr : attribs)
                    {
                        if (rch.mAttrib == attr)
                        {
                            rChans.push_back(&rch);
                            attributeCount += rch.mComponentCount;
                            cc.mChannelDataFlags = (U8)getElemSize(rch.mElementSize);
                            break;
                        }
                    }
                }
                U64 size = ((cc.mVertexCount * attributeCount * U64(cc.mBitsPerIndex)) * 7ull) / 8ull;

                U8* bitData = new U8[size];
                utils::BitStreamWriter bsw(bitData, size);

                if (!rChans.empty())
                {
                    for (size_t i = 0u; i < cc.mVertexCount; i++)
                    {
                        for (const auto rch : rChans)
                        {
                            size_t off = i * rch->mComponentCount;
                            for (size_t j = 0u; j < rch->mComponentCount; j++)
                            {
                                U32* fltData = (U32*)rch->mData;
                                auto it = std::lower_bound(v.begin(), v.end(), fltData[off + j]);
                                U32 idx = (U32)(it - v.begin());
                                bsw.writeBits(idx, cc.mBitsPerIndex);
                            }
                        }
                    }
                }

                bsw.flush();
                cc.mBitData.resize(bsw.getWrittenSize());
                memcpy(cc.mBitData.data(), bitData, bsw.getWrittenSize());
                utils::SetBit(cc.mChannelAttribute, static_cast<U8>(Attrib::Compressed));
                delete[] bitData;

                break;
            }
            case Type::tU8:
            {
                TSet<U8>* ts = (TSet<U8>*)s;
                std::vector<U8> v = std::vector<U8>(ts->mSet.begin(), ts->mSet.end());
                cc.mElementCount = U32(v.size());
                cc.mData.resize(v.size() * sizeof(U8));
                memcpy(cc.mData.data(), v.data(), v.size() * sizeof(U8));
                cc.mBitsPerIndex = utils::bits_needed(cc.mElementCount - 1u);
                cc.mVertexCount = ts->mVertexCount;
                cc.mChannelAttribute = ts->mAttribute;

                std::vector<const RawChannel*> rChans;
                U64 attributeCount = 0u;
                auto attribs = types::GetAttributes(ts->mAttribute);
                for (const auto& rch : rawChannels)
                {
                    for (const auto& attr : attribs)
                    {
                        if (rch.mAttrib == attr)
                        {
                            rChans.push_back(&rch);
                            attributeCount += rch.mComponentCount;
                            cc.mChannelDataFlags = (U8)getElemSize(rch.mElementSize);
                            break;
                        }
                    }
                }
                U64 size = ((cc.mVertexCount * attributeCount * U64(cc.mBitsPerIndex)) * 7ull) / 8ull;

                U8* bitData = new U8[size];
                utils::BitStreamWriter bsw(bitData, size);

                if (!rChans.empty())
                {
                    for (size_t i = 0u; i < cc.mVertexCount; i++)
                    {
                        for (const auto rch : rChans)
                        {
                            size_t off = i * rch->mComponentCount;
                            for (size_t j = 0u; j < rch->mComponentCount; j++)
                            {
                                U8* fltData = (U8*)rch->mData;
                                auto it = std::lower_bound(v.begin(), v.end(), fltData[off + j]);
                                U32 idx = (U32)(it - v.begin());
                                bsw.writeBits(idx, cc.mBitsPerIndex);
                            }
                        }
                    }
                }

                bsw.flush();
                cc.mBitData.resize(bsw.getWrittenSize());
                memcpy(cc.mBitData.data(), bitData, bsw.getWrittenSize());
                utils::SetBit(cc.mChannelAttribute, static_cast<U8>(Attrib::Compressed));
                delete[] bitData;

                break;
            }
            default:
                break;
            }

            compressedChannels.emplace_back(cc);
            delete s;
        }

        return Result::Success;
    }

    Result VTSaverV1_x::saveChannels(const std::vector<CompressedChannel>& channels)
    {
        U8 channelCount = static_cast<U8>(channels.size());
        mCommonLdr.saveNumber((U8)0u);
        Result res = mCommonLdr.saveNumber(channelCount);
        for (const auto& ch : channels)
        {
            Result r = Ok(res) ? mCommonLdr.saveNumber(ch.mChannelAttribute) : res;
            const SwBool compressedChannel = utils::IsBitSet(ch.mChannelAttribute, (U8)types::compressFlags::CompressedChannelAttribute::Compressed);

            if (compressedChannel)
            {
                r = Ok(r) ? mCommonLdr.saveNumber(ch.mChannelDataFlags) : r;
                r = Ok(r) ? mCommonLdr.saveNumber(ch.mElementCount) : r;
                r = Ok(r) ? mCommonLdr.saveArray(ch.mData) : r;
                r = Ok(r) ? mCommonLdr.saveNumber(ch.mBitsPerIndex) : r;
                r = Ok(r) ? mCommonLdr.saveNumber(ch.mVertexCount) : r;
                r = Ok(r) ? mCommonLdr.saveArray(ch.mBitData) : r;
            }
            else
            {
                r = Ok(r) ? mCommonLdr.saveNumber(ch.mVertexCount) : r;
                r = Ok(r) ? mCommonLdr.saveArray(ch.mData) : r;
            }
            res = r;
        }

        return res;
    }
}
