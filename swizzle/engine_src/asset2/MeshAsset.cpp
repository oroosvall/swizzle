
/* Include files */

#include "MeshAsset.hpp"
#include "SwmMeshAsset.hpp"
#include "ObjMeshAsset.hpp"
#include "AssetUtils.hpp"

#include <swizzle/core/Logging.hpp>

#include <vector>
#include <map>
#include <algorithm>

/* Defines */

/* Typedefs */

/* Structs/Classes */

namespace swizzle::asset2
{
    struct OffsetItem
    {
        U32 mOffset;
        U64 mSize;
    };
}

/* Static Variables */

/* Static Function Declaration */

namespace swizzle::asset2
{
    static SwBool ValidateLoadInfo(MeshAssetLoaderDescription& loadInfo);
}

/* Static Function Definition */

namespace swizzle::asset2
{
    static SwBool ValidateLoadInfo(MeshAssetLoaderDescription& loadInfo)
    {
        std::map<swizzle::asset2::AttributeTypes, U32> attribCount;
        std::vector<OffsetItem> offsetList;

        SwBool hasPositions = false;

        for (auto it = loadInfo.mLoadPossitions.begin(); it != loadInfo.mLoadPossitions.end(); ++it)
        {
            if (it->mAttributeType == swizzle::asset2::AttributeTypes::VertexPosition)
            {
                hasPositions = true;
            }
            offsetList.push_back(OffsetItem{ it->mByteOffset, AttributeSize(it->mAttributeType) });
            auto attr = attribCount.find(it->mAttributeType);

            if (attr != attribCount.end())
            {
                attr->second++;
            }
            else
            {
                attribCount.insert(std::make_pair(it->mAttributeType, 1));
            }
        }

        SwBool occursOnce = true;

        for (auto& it : attribCount)
        {
            if (it.second != 1)
            {
                occursOnce = false;
                break;
            }
        }

        SwBool noOverlap = true;
        std::sort(offsetList.begin(), offsetList.end(), [](const OffsetItem& a, const OffsetItem& b) {
            return a.mOffset < b.mOffset;
                  });
        U64 off = 0u;
        for (auto& it : offsetList)
        {
            if (off != it.mOffset)
            {
                noOverlap = false;
            }
            off += it.mSize;
        }

        return hasPositions && occursOnce && noOverlap;
    }
}

/* Function Definition */

namespace swizzle::asset2
{
    common::Resource<IMeshAsset> LoadMesh(const SwChar* fileName, MeshAssetLoaderDescription& loadInfo)
    {
        if (!ValidateLoadInfo(loadInfo))
        {
            return nullptr;
        }

        LOG_INFO("Loading mesh %s", fileName);

        common::Resource<IMeshAsset> asset = LoadSwmMesh(fileName, loadInfo);
        if (asset)
        {
            return asset;
        }

        asset = LoadObjMesh(fileName, loadInfo);
        if (asset)
        {
            return asset;
        }

        return nullptr;
    }
}

/* Class Public Function Definition */

namespace swizzle::asset2
{
    MeshAsset::MeshAsset(VertexData& vertexData, IndexData& indexData, AnimationDataInformation* animationInfo)
        : mVertexData(std::move(vertexData))
        , mIndexData(std::move(indexData))
        , mHasAnimationData(animationInfo)
    {
        if (mHasAnimationData)
        {
            mNumBoneCount = animationInfo->mNumBoneCount;
            mAnimations = std::move(animationInfo->mAnimations);
        }
    }

    MeshAsset::~MeshAsset()
    {
    }

    U64 MeshAsset::getVertexDataSize() const
    {
        return mVertexData.size();
    }

    const U8* MeshAsset::getVertexDataPtr() const
    {
        return mVertexData.data();
    }

    SwBool MeshAsset::hasIndexData() const
    {
        return mIndexData.size() != 0ull;
    }

    U64 MeshAsset::getIndexDataSize() const
    {
        return mIndexData.size();
    }

    const U8* MeshAsset::getIndexDataPtr() const
    {
        return mIndexData.data();
    }

    SwBool MeshAsset::hasAnimations() const
    {
        return mHasAnimationData;
    }

    U32 MeshAsset::getNumberOfAnimations() const
    {
        return (U32)mAnimations.size();
    }

    U32 MeshAsset::getNumberOfBones() const
    {
        return mNumBoneCount;
    }

    U32 MeshAsset::getNumberOfKeyFrames(U32 animationIndex) const
    {
        return (U32)mAnimations[animationIndex].mKeyFrames.size();
    }

    const SwChar* MeshAsset::getAnimationName(U32 animationIndex) const
    {
        return mAnimations[animationIndex].mName.c_str();
    }

    const U8* MeshAsset::getAnimationDataPtr(U32 animationIndex, U32 keyFrameIndex) const
    {
        return (const U8*)mAnimations[animationIndex].mKeyFrames[keyFrameIndex].mFrameData.data();
    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
