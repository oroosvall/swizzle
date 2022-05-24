#ifndef MESH_ASSET_IMPL_HPP
#define MESH_ASSET_IMPL_HPP

/* Include files */

#include <swizzle/asset2/Assets.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::asset2
{
    struct VertexInformation
    {
        U64 mVertexDataSize;
        U8* mVertexData;
    };

    struct IndexInformation
    {
        U64 mIndexDataSize;
        U8* mIndexData;
    };

    struct AnimationDataInformation
    {

    };
}

/* Class Declaration */

namespace swizzle::asset2
{
    class MeshAsset : public IMeshAsset
    {
    public:
        MeshAsset(VertexInformation vertexInfo, IndexInformation* indexInfo, AnimationDataInformation* animationInfo);
        virtual ~MeshAsset();

        virtual U64 getVertexDataSize() const override;
        virtual U8* getVertexDataPtr() const override;

        virtual SwBool hasIndexData() const override;
        virtual U64 getIndexDataSize() const override;
        virtual U8* getIndexDataPtr() const override;

        virtual SwBool hasAnimations() const override;
        virtual U32 getNumberOfAnimations() const override;
        virtual U32 getNumberOfBones() const override;
        virtual U32 getNumberOfKeyFrames(U32 animationIndex) const override;
        virtual U8* getAnimationDataPtr(U32 animationIndex, U32 keyFrameIndex) const override;
    private:

        U64 mVertexDataSize;
        U8* mVertexData;

        SwBool mHasIndexData;
        U64 mIndexDataSize;
        U8* mIndexData;

        SwBool mHasAnimationData;

    };
}

/* Function Declaration */

#endif
