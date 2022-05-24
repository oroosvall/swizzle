
/* Include files */

#include "MeshAsset.hpp"

#include <swm/Swm.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::asset2
{
    common::Resource<IMeshAsset> LoadMesh(const SwChar* fileName, MeshAssetLoaderDescription& loadInfo)
    {
        swm::Model mdl;
        swm::LoadResult res = swm::LoadSwm(fileName, mdl);

        common::Resource<IMeshAsset> loadedModel = nullptr; 

        if (res != swm::LoadResult::Success)
        {
                
        }

        return loadedModel;
    }
}

/* Class Public Function Definition */

namespace swizzle::asset2
{
    MeshAsset::MeshAsset(VertexInformation vertexInfo, IndexInformation* indexInfo, AnimationDataInformation* animationInfo)
        : mVertexData(vertexInfo.mVertexData)
        , mVertexDataSize(vertexInfo.mVertexDataSize)
        , mHasIndexData(indexInfo)
        , mIndexData(nullptr)
        , mIndexDataSize(0ull)
        , mHasAnimationData(animationInfo)
    {
        if (mHasIndexData)
        {
            mIndexData = indexInfo->mIndexData;
            mIndexDataSize = indexInfo->mIndexDataSize;
        }
    }

    MeshAsset::~MeshAsset()
    {
        delete [] mVertexData;
        delete [] mIndexData;
    }

    U64 MeshAsset::getVertexDataSize() const
    {
        return mVertexDataSize;
    }

    U8* MeshAsset::getVertexDataPtr() const
    {
        return mVertexData;
    }

    SwBool MeshAsset::hasIndexData() const
    {
        return mHasIndexData;    
    }

    U64 MeshAsset::getIndexDataSize() const
    {
        return mIndexDataSize;
    }

    U8* MeshAsset::getIndexDataPtr() const
    {
        return mIndexData;
    }

    SwBool MeshAsset::hasAnimations() const
    {
        return mHasAnimationData;
    }

    U32 MeshAsset::getNumberOfAnimations() const
    {
        return 0ull;
    }

    U32 MeshAsset::getNumberOfBones() const 
    {
        return 0ull;
    }

    U32 MeshAsset::getNumberOfKeyFrames(U32 animationIndex) const
    {
        UNUSED_ARG(animationIndex);
        return 0ull;
    }

    U8* MeshAsset::getAnimationDataPtr(U32 animationIndex, U32 keyFrameIndex) const
    {
        UNUSED_ARG(animationIndex);
        UNUSED_ARG(keyFrameIndex);

        return 0ull;
    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
