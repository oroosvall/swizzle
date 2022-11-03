#ifndef LOAD_V1_X_HPP
#define LOAD_V1_X_HPP

/* Include files */

#include "Load.hpp"
#include "ChannelCommon.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

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
        Result loadChannel(channel::CompressedChannel& channel, SwBool hasAnimations);
        Result processChannel(const channel::CompressedChannel& ch, types::Mesh& mesh);
        types::mappingFlags::MappingBits getChannelMappingBits(U8 channelFlags);

        LoaderCommonIfc& mCommonLdr;
        types::Header mHeader;
    };

} // namespace swm::load

/* Function Declaration */

#endif
