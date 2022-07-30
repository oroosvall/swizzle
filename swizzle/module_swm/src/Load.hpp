#ifndef LOAD_HPP
#define LOAD_HPP

/* Include files */

#include <swm/Swm.hpp>
#include "helpers/FileReader.hpp"
#include "InternalStructs.hpp"

#include <string>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swm::load
{
    class LoaderCommonIfc
    {
    public:
        virtual ~LoaderCommonIfc() = default;

        virtual Result loadShortString(std::string& str) = 0;

        virtual Result loadNumber(U8& num) = 0;
        virtual Result loadNumber(U16& num) = 0;
        virtual Result loadNumber(U32& num) = 0;

        virtual Result loadArray(std::vector<U8>& vec, size_t count) = 0;
        virtual Result loadArray(std::vector<U16>& vec, size_t count) = 0;
        virtual Result loadArray(std::vector<F32>& vec, size_t count) = 0;

        virtual Result loadArray(std::vector<types::Vector3F>& vec, size_t count) = 0;
        virtual Result loadArray(std::vector<types::Vector2F>& vec, size_t count) = 0;
        virtual Result loadArray(std::vector<types::Color4U>& vec, size_t count) = 0;
        virtual Result loadArray(std::vector<types::BoneIndex>& vec, size_t count) = 0;
        virtual Result loadArray(std::vector<types::BoneWeights>& vec, size_t count) = 0;
        virtual Result loadArray(std::vector<types::Triangle>& vec, size_t count) = 0;
        virtual Result loadArray(std::vector<types::Matrix4x4>& vec, size_t count) = 0;
    };

    class LoaderCommon : public LoaderCommonIfc
    {
    public:
        LoaderCommon(file::FileReader& fileReader);
        virtual ~LoaderCommon() = default;

        virtual Result loadShortString(std::string& str) override;

        virtual Result loadNumber(U8& num) override;
        virtual Result loadNumber(U16& num) override;
        virtual Result loadNumber(U32& num) override;

        virtual Result loadArray(std::vector<U8>& vec, size_t count) override;
        virtual Result loadArray(std::vector<U16>& vec, size_t count) override;
        virtual Result loadArray(std::vector<F32>& vec, size_t count) override;

        virtual Result loadArray(std::vector<types::Vector3F>& vec, size_t count) override;
        virtual Result loadArray(std::vector<types::Vector2F>& vec, size_t count) override;
        virtual Result loadArray(std::vector<types::Color4U>& vec, size_t count) override;
        virtual Result loadArray(std::vector<types::BoneIndex>& vec, size_t count) override;
        virtual Result loadArray(std::vector<types::BoneWeights>& vec, size_t count) override;
        virtual Result loadArray(std::vector<types::Triangle>& vec, size_t count) override;
        virtual Result loadArray(std::vector<types::Matrix4x4>& vec, size_t count) override;

    private:
        file::FileReader& mFileReader;

        template<typename T>
        Result tLoadNum(T& num)
        {
            return mFileReader.readNum(num) ? Result::Success : Result::ErrFileIo;
        }

        template<typename T>
        Result tLoadArray(std::vector<T>& vec, size_t count)
        {
            return mFileReader.readArray(vec, count) ? Result::Success : Result::ErrFileIo;
        }

    };

    class MeshLoaderIfc
    {
    public:
        virtual ~MeshLoaderIfc() = default;

        virtual Result loadMeshHeader(U32& num) = 0;
        virtual Result loadMeshData(types::Mesh& mesh) = 0;
    };

    class VTLoaderIfc
    {
    public:
        virtual ~VTLoaderIfc() = default;

        virtual Result loadVertexData(types::Mesh& mesh) = 0;
        virtual Result loadTriangleData(types::Mesh& mesh) = 0;

        virtual Result loadAnimationData(types::Mesh& mesh) = 0;

        virtual Result loadVertexDataCompressed(types::Mesh& mesh) = 0;
        virtual Result loadTriangleDataCompressed(types::Mesh& mesh) = 0;
        virtual Result loadAnimationDataCompressed(types::Mesh& mesh) = 0;

    };

}

/* Function Declaration */

namespace swm::load
{
    Result LoadModel(file::FileReader& fr, Model& mdl);

    Result LoadHeader(file::FileReader& fr, types::Header& header);
    Result ValidateHeaderMagic(const types::Header& hdr);

    Result GetLoadFunctions(const types::Header& hdr, MeshLoaderIfc** meshLdr, VTLoaderIfc** vtLdr, LoaderCommon& ldr);

    Result LoadData(MeshLoaderIfc& ldr, Model& mdl);
}

#endif
