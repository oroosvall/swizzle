
/* Include files */

#include "Save.hpp"
#include "SaveV1_x.hpp"

#include "Ok.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace swm::save
{
    Result SaveModel(file::FileWriter& fw, const Model& mdl, const options::SaveOptions& options)
    {
        UNUSED_ARG(options);

        // Get version to save
        types::Header header = {};
        header.mMagic[0] = 'S';
        header.mMagic[1] = 'W';
        header.mMagic[2] = 'M';
        header.mMagic[3] = ' ';

        header.mVersion.mMajor = 1u;
        header.mVersion.mMinor = 0u;

        // Write Header and version
        Result res = SaveHeader(fw, header);

        // Get saver instance
        VTSaverIfc* vtSaver = nullptr;
        MeshSaverIfc* modelSaver = nullptr;

        SaverCommon saverCommon = { fw };
        vtSaver = new VTSaverV1_x(saverCommon, header);
        modelSaver = new MeshSaverV1_x(saverCommon, *vtSaver, header);

        // Write all model data
        res = Ok(res) ? SaveData(*modelSaver, mdl, options) : res;

        return Result::Success;
    }

    Result SaveHeader(file::FileWriter& fw, const types::Header& header)
    {
        return fw.write(header) ? Result::Success : Result::ErrFileIo;
    }


    Result SaveData(MeshSaverIfc& meshSaver, const Model& mdl, const options::SaveOptions& options)
    {
        Result res = meshSaver.saveMeshHeader(static_cast<U32>(mdl.mMeshes.size()));
        for(const types::Mesh& m : mdl.mMeshes)
        {
            res = Ok(res) ? meshSaver.saveMeshData(m, options) : res;
            if (res != Result::Success)
            {
                break;
            }
        }
        return res;
    }
}

/* Class Public Function Definition */

namespace swm::save
{

    SaverCommon::SaverCommon(file::FileWriter& fileWriter)
        : mFileWriter(fileWriter)
    {
    }

    Result SaverCommon::saveShortString(const std::string& str)
    {
        U8 count = static_cast<U8>(str.size());
        if (str.size() > 255u)
        {
            count = 255u;
        }

        mFileWriter.write(count);
        mFileWriter.writeArray(str.data(), count);

        return Result::Success;
    }

    Result SaverCommon::saveNumber(const U8 num)
    {
        return tSaveNum(num);
    }

    Result SaverCommon::saveNumber(const U16 num)
    {
        return tSaveNum(num);
    }

    Result SaverCommon::saveNumber(const U32 num)
    {
        return tSaveNum(num);
    }

    Result SaverCommon::saveArray(const std::vector<U8>& vec)
    {
        return tSaveArray(vec);
    }

    Result SaverCommon::saveArray(const std::vector<U16>& vec)
    {
        return tSaveArray(vec);
    }

    Result SaverCommon::saveArray(const std::vector<types::Vector3F>& vec)
    {
        return tSaveArray(vec);
    }

    Result SaverCommon::saveArray(const std::vector<types::Vector2F>& vec)
    {
        return tSaveArray(vec);
    }

    Result SaverCommon::saveArray(const std::vector<types::Color4U>& vec)
    {
        return tSaveArray(vec);
    }

    Result SaverCommon::saveArray(const std::vector<types::BoneIndex>& vec)
    {
        return tSaveArray(vec);
    }

    Result SaverCommon::saveArray(const std::vector<types::BoneWeights>& vec)
    {
        return tSaveArray(vec);
    }

    Result SaverCommon::saveArray(const std::vector<types::Triangle>& vec)
    {
        return tSaveArray(vec);
    }

    Result SaverCommon::saveArray(const std::vector<types::Matrix4x4>& vec)
    {
        return tSaveArray(vec);
    }

    Result SaverCommon::saveArray(const U8* data, size_t count)
    {
        return mFileWriter.writeArray(data, count) ? Result::Success : Result::ErrFileIo;
    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
