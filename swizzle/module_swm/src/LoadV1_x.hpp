#ifndef LOAD_V1_X_HPP
#define LOAD_V1_X_HPP

/* Include files */

#include "Load.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swm::load
{
    struct CompressedChannel
    {
        U8 mChannelAttribute;
        U8 mChannelDataFlags;
        U32 mElementCount;
        std::vector<U8> mData;
        U8 mBitsPerIndex;
        U32 mVertexCount;
        std::vector<U8> mBitData;
    };
}

/* Class Declaration */

namespace swm::load
{
    class MeshLoaderV1_x : public MeshLoaderIfc
    {
    public:
        MeshLoaderV1_x(LoaderCommonIfc& commonLdr, VTLoaderIfc& vtLoader, const types::Header& header);
        virtual ~MeshLoaderV1_x() = default;

        virtual Result loadMeshHeader(U32& num) override;
        virtual Result loadMeshData(types::Mesh& mesh) override;

    private:
        LoaderCommonIfc& mCommonLdr;
        VTLoaderIfc& mVtLoader;
        types::Header mHeader;
    };

    class VTLoaderV1_x : public VTLoaderIfc
    {
    public:
        VTLoaderV1_x(LoaderCommonIfc& commonLdr, const types::Header& header);
        virtual ~VTLoaderV1_x() = default;

        virtual Result loadVertexData(types::Mesh& mesh) override;
        virtual Result loadTriangleData(types::Mesh& mesh) override;

        virtual Result loadAnimationData(types::Mesh& mesh) override;

        virtual Result loadVertexDataCompressed(types::Mesh& mesh) override;
        virtual Result loadTriangleDataCompressed(types::Mesh& mesh) override;
        virtual Result loadAnimationDataCompressed(types::Mesh& mesh) override;

    private:

        Result loadChannel(CompressedChannel& channel, SwBool hasAnimations);
        U64 getChannelElemetSize(U8 channelFlags);
        Result processChannel(const CompressedChannel& ch, types::Mesh& mesh);
        types::mappingFlags::MappingBits getChannelMappingBits(U8 channelFlags);

        LoaderCommonIfc& mCommonLdr;
        types::Header mHeader;
    };

}

/* Function Declaration */

#endif
