#ifndef ASSET_HPP
#define ASSET_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/Api.hpp>
#include <swizzle/core/DataBuffer.hpp>

/* Defines */

/* Typedefs/enums */

namespace swizzle::asset2
{
    enum class AttributeTypes
    {
        VertexPosition = 0,
        UvCoordinates,
        NormalVector,
        BoneIndices,
        BoneWeights,
        VertexColor
    };
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::asset2
{
    class VertexLoadPosition
    {
    public:
        VertexLoadPosition(AttributeTypes attribute, U32 offset) : mAttributeType(attribute), mByteOffset(offset) { }
        virtual ~VertexLoadPosition() { }

        AttributeTypes mAttributeType; // The attribute that will be loaded
        U32 mByteOffset;  // the offset where the attribute will be loaded
    };

    struct MeshAssetLoaderDescription
    {
        common::IterType<VertexLoadPosition> mLoadPossitions;
    };

    class IMeshAsset
    {
    public:
        virtual ~IMeshAsset() { }

        virtual U64 getVertexDataSize() const = 0;
        virtual const U8* getVertexDataPtr() const = 0;

        virtual SwBool hasIndexData() const = 0;
        virtual U64 getIndexDataSize() const = 0;
        virtual const U8* getIndexDataPtr() const = 0;

        virtual SwBool hasAnimations() const = 0;
        virtual U32 getNumberOfAnimations() const = 0;
        virtual U32 getNumberOfBones() const = 0;
        virtual U32 getNumberOfKeyFrames(U32 animationIndex) const = 0;
        virtual const SwChar* getAnimationName(U32 animationIndex) const = 0;
        virtual const U8* getAnimationDataPtr(U32 animationIndex, U32 keyFrameIndex) const = 0;

    };
}

/* Class Declaration */

/* Function Declaration */

namespace swizzle::asset2
{
    common::Resource<IMeshAsset> SWIZZLE_API LoadMesh(const SwChar* fileName, MeshAssetLoaderDescription& loadInfo);
}

#endif
