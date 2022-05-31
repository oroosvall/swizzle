#ifndef MESH_ASSET_IMPL_HPP
#define MESH_ASSET_IMPL_HPP

/* Include files */

#include "MeshAnimation.hpp"

#include <swizzle/asset2/Assets.hpp>

#include <vector>

/* Defines */

/* Typedefs/enums */

namespace swizzle::asset2
{
    typedef std::vector<U8> VertexData;
    typedef std::vector<U8> IndexData;
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::asset2
{
    struct AnimationDataInformation
    {
        U32 mNumBoneCount;
        std::vector<Animation> mAnimations;
    };
}

/* Class Declaration */

namespace swizzle::asset2
{
    class MeshAsset : public IMeshAsset
    {
    public:
        MeshAsset(VertexData& vertexInfo, IndexData& indexInfo, AnimationDataInformation* animationInfo);
        virtual ~MeshAsset();

        virtual U64 getVertexDataSize() const override;
        virtual const U8* getVertexDataPtr() const override;

        virtual SwBool hasIndexData() const override;
        virtual U64 getIndexDataSize() const override;
        virtual const U8* getIndexDataPtr() const override;

        virtual SwBool hasAnimations() const override;
        virtual U32 getNumberOfAnimations() const override;
        virtual U32 getNumberOfBones() const override;
        virtual U32 getNumberOfKeyFrames(U32 animationIndex) const override;
        virtual const SwChar* getAnimationName(U32 animationIndex) const override;
        virtual const U8* getAnimationDataPtr(U32 animationIndex, U32 keyFrameIndex) const override;
    private:

        VertexData mVertexData;

        IndexData mIndexData;

        SwBool mHasAnimationData;
        U32 mNumBoneCount;

        std::vector<Animation> mAnimations;

    };
}

/* Function Declaration */

#endif
