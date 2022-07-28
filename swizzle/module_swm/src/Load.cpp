
/* Include files */

#include "Load.hpp"
#include "LoadV1_x.hpp"

#include "Ok.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace swm::load
{
    Result LoadModel(file::FileReader& fr, Model& mdl)
    {
        types::Header hdr{};

        LoaderCommon ldr = { fr };
        // Don't like this pointer stuff
        VTLoaderIfc* vtLoader = nullptr;
        MeshLoaderIfc* meshLoader = nullptr;

        Result res = LoadHeader(fr, hdr);
        res = Ok(res) ? ValidateHeaderMagic(hdr) : res;
        res = Ok(res) ? GetLoadFunctions(hdr, &meshLoader, &vtLoader, ldr) : res;

        res = (Ok(res) && meshLoader) ? LoadData(*meshLoader, mdl) : res;

        delete meshLoader;
        delete vtLoader;

        return res;
    }

    Result LoadHeader(file::FileReader& fr, types::Header& header)
    {
        return fr.readNum(header) ? Result::Success : Result::ErrFileIo;
    }

    Result ValidateHeaderMagic(const types::Header& hdr)
    {
        if (((hdr.mMagic[0] == 'S') &&
             (hdr.mMagic[1] == 'W') &&
             (hdr.mMagic[2] == 'M') &&
             (hdr.mMagic[3] == ' ')))
        {
            return Result::Success;
        }
        return Result::ErrMagic;
    }

    Result GetLoadFunctions(const types::Header& hdr, MeshLoaderIfc** meshLdr, VTLoaderIfc** vtLdr, LoaderCommon& ldr)
    {
        switch (hdr.mVersion.mMajor)
        {
        case 1u:
        {
            // Don't like this pointer stuff
            *vtLdr = new VTLoaderV1_x(ldr, hdr);
            *meshLdr = new MeshLoaderV1_x(ldr, **vtLdr, hdr);
            return Result::Success;
        }
        default:
            return Result::ErrVersion;
        }
    }

    Result LoadData(MeshLoaderIfc& ldr, Model& mdl)
    {
        U32 meshCount = 0u;
        Result res = ldr.loadMeshHeader(meshCount);

        for (U32 i = 0u; (i < meshCount) && Ok(res); ++i)
        {
            types::Mesh mesh;
            res = ldr.loadMeshData(mesh);
            mdl.mMeshes.push_back(mesh);
        }

        return res;
    }
}

/* Class Public Function Definition */

namespace swm::load
{
    LoaderCommon::LoaderCommon(file::FileReader& fileReader)
        : mFileReader(fileReader)
    {
    }

    Result LoaderCommon::loadShortString(std::string& str)
    {
        U8 count = 0u;
        Result res = mFileReader.readNum(count) ? Result::Success : Result::ErrFileIo;
        if (Ok(res))
        {
            std::vector<U8> d;
            res = loadArray(d, count);
            str = std::string((char*)d.data(), d.size());
        }
        return res;
    }

    Result LoaderCommon::loadNumber(U8& num)
    {
        return tLoadNum(num);
    }

    Result LoaderCommon::loadNumber(U16& num)
    {
        return tLoadNum(num);
    }

    Result LoaderCommon::loadNumber(U32& num)
    {
        return tLoadNum(num);
    }

    Result LoaderCommon::loadArray(std::vector<U8>& vec, size_t count)
    {
        return tLoadArray(vec, count);
    }

    Result LoaderCommon::loadArray(std::vector<U16>& vec, size_t count)
    {
        return tLoadArray(vec, count);
    }

    Result LoaderCommon::loadArray(std::vector<types::Vector3F>& vec, size_t count)
    {
        return tLoadArray(vec, count);
    }

    Result LoaderCommon::loadArray(std::vector<types::Vector2F>& vec, size_t count)
    {
        return tLoadArray(vec, count);
    }

    Result LoaderCommon::loadArray(std::vector<types::Color4U>& vec, size_t count)
    {
        return tLoadArray(vec, count);
    }

    Result LoaderCommon::loadArray(std::vector<types::BoneIndex>& vec, size_t count)
    {
        return tLoadArray(vec, count);
    }

    Result LoaderCommon::loadArray(std::vector<types::BoneWeights>& vec, size_t count)
    {
        return tLoadArray(vec, count);
    }

    Result LoaderCommon::loadArray(std::vector<types::Triangle>& vec, size_t count)
    {
        return tLoadArray(vec, count);
    }

    Result LoaderCommon::loadArray(std::vector<types::Matrix4x4>& vec, size_t count)
    {
        return tLoadArray(vec, count);
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */
