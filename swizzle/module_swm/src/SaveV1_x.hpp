#ifndef SAVE_V1_X_HPP
#define SAVE_V1_X_HPP

/* Include files */

#include "Save.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swm::save
{
    struct RawChannel
    {
        U64 mElementCount;
        U8* mData;
        U64 mDataSize;
        U8 mElementSize;
        U8 mComponentCount;
        types::compressFlags::CompressedChannelAttribute mAttrib;
    };

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
} // namespace swm::save

/* Class Declaration */

namespace swm::save
{
    class MeshSaverV1_x : public MeshSaverIfc
    {
    public:
        MeshSaverV1_x(SaverCommonIfc& commonSaver, VTSaverIfc& vtSaver, const types::Header& header);
        virtual ~MeshSaverV1_x() = default;

        virtual Result saveMeshHeader(const U32& num) override;
        virtual Result saveMeshData(const types::Mesh& mesh, const options::SaveOptions& options) override;

    private:
        SaverCommonIfc& mCommonSaver;
        VTSaverIfc& mVtSaver;
        const types::Header mHeader;
    };

    class VTSaverV1_x : public VTSaverIfc
    {
    public:
        VTSaverV1_x(SaverCommonIfc& commonSaver, const types::Header& header);
        virtual ~VTSaverV1_x() = default;

        virtual Result saveVertexData(const types::Mesh& mesh, const U16 flags) override;
        virtual Result saveTriangleData(const types::Mesh& mesh, const U16 flags) override;

        virtual Result saveAnimationData(const types::Mesh& mesh) override;

        virtual Result saveVertexDataCompressed(const types::Mesh& mesh, const U16 flags) override;
        virtual Result saveTriangleDataCompressed(const types::Mesh& mesh) override;
        virtual Result saveAnimationDataCompressed(const types::Mesh& mesh) override;

    private:
        Result createChannels(std::vector<RawChannel>& channels, const types::Mesh& mesh, const U16 flags);
        Result buildChannels(const std::vector<RawChannel>& rawChannels,
                             std::vector<CompressedChannel>& compressedChannels);
        Result saveChannels(const std::vector<CompressedChannel>& channels);

        SaverCommonIfc& mCommonSaver;
        const types::Header mHeader;
    };

} // namespace swm::save

/* Function Declaration */

#endif
