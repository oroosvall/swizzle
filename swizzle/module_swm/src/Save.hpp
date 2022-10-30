#ifndef SAVE_HPP
#define SAVE_HPP

/* Include files */

#include "InternalStructs.hpp"
#include "helpers/FileWriter.hpp"
#include <swm/Swm.hpp>

#include <string>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swm::save
{
    class SaverCommonIfc
    {
    public:
        virtual ~SaverCommonIfc() = default;

        virtual Result saveShortString(const std::string& str) = 0;

        virtual Result saveNumber(const U8 num) = 0;
        virtual Result saveNumber(const U16 num) = 0;
        virtual Result saveNumber(const U32 num) = 0;

        virtual Result saveArray(const std::vector<U8>& vec) = 0;
        virtual Result saveArray(const std::vector<U16>& vec) = 0;

        virtual Result saveArray(const std::vector<types::Vector3F>& vec) = 0;
        virtual Result saveArray(const std::vector<types::Vector2F>& vec) = 0;
        virtual Result saveArray(const std::vector<types::Color4U>& vec) = 0;
        virtual Result saveArray(const std::vector<types::BoneIndex>& vec) = 0;
        virtual Result saveArray(const std::vector<types::BoneWeights>& vec) = 0;
        virtual Result saveArray(const std::vector<types::Triangle>& vec) = 0;
        virtual Result saveArray(const std::vector<types::Matrix4x4>& vec) = 0;

        virtual Result saveArray(const U8* data, size_t count) = 0;
    };

    class SaverCommon : public SaverCommonIfc
    {
    public:
        SaverCommon(file::FileWriter& fileWriter);
        virtual ~SaverCommon() = default;

        virtual Result saveShortString(const std::string& str) override;

        virtual Result saveNumber(const U8 num) override;
        virtual Result saveNumber(const U16 num) override;
        virtual Result saveNumber(const U32 num) override;

        virtual Result saveArray(const std::vector<U8>& vec) override;
        virtual Result saveArray(const std::vector<U16>& vec) override;

        virtual Result saveArray(const std::vector<types::Vector3F>& vec) override;
        virtual Result saveArray(const std::vector<types::Vector2F>& vec) override;
        virtual Result saveArray(const std::vector<types::Color4U>& vec) override;
        virtual Result saveArray(const std::vector<types::BoneIndex>& vec) override;
        virtual Result saveArray(const std::vector<types::BoneWeights>& vec) override;
        virtual Result saveArray(const std::vector<types::Triangle>& vec) override;
        virtual Result saveArray(const std::vector<types::Matrix4x4>& vec) override;

        virtual Result saveArray(const U8* data, size_t count) override;

    private:
        file::FileWriter& mFileWriter;

        template <typename T>
        Result tSaveNum(T num)
        {
            return mFileWriter.write(num) ? Result::Success : Result::ErrFileIo;
        }

        template <typename T>
        Result tSaveArray(const std::vector<T>& vec)
        {
            return mFileWriter.writeArray(vec.data(), vec.size()) ? Result::Success : Result::ErrFileIo;
        }
    };

    class MeshSaverIfc
    {
    public:
        virtual ~MeshSaverIfc() = default;

        virtual Result saveMeshHeader(const U32& num) = 0;
        virtual Result saveMeshData(const types::Mesh& mesh, const options::SaveOptions& options) = 0;
    };

    class VTSaverIfc
    {
    public:
        virtual ~VTSaverIfc() = default;

        virtual Result saveVertexData(const types::Mesh& mesh, const U16 flags) = 0;
        virtual Result saveTriangleData(const types::Mesh& mesh, const U16 flags) = 0;

        virtual Result saveAnimationData(const types::Mesh& mesh) = 0;

        virtual Result saveVertexDataCompressed(const types::Mesh& mesh, const U16 flags) = 0;
        virtual Result saveTriangleDataCompressed(const types::Mesh& mesh) = 0;
        virtual Result saveAnimationDataCompressed(const types::Mesh& mesh) = 0;
    };

} // namespace swm::save

/* Function Declaration */

namespace swm::save
{
    Result SaveModel(file::FileWriter& fw, const Model& mdl, const options::SaveOptions& options);

    Result SaveHeader(file::FileWriter& fw, const types::Header& header);

    Result SaveData(MeshSaverIfc& meshSaver, const Model& mdl, const options::SaveOptions& options);
} // namespace swm::save

#endif
