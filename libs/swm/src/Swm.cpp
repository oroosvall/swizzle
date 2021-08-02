
/* Include files */

#include <swm/Swm.hpp>
#include "Internal.hpp"

#include <fstream>

#include <utils/BufferReader.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swm
{
    LoadResult LoadSwm(std::filesystem::path filePath, Model& model)
    {
        LoadResult res = LoadResult::FileIoError;

        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (file.is_open())
        {
            size_t length = file.tellg();
            file.clear();
            file.seekg(0, std::ios::beg);

            std::vector<U8> fileData(length);
            file.read((char*)fileData.data(), length);
            size_t readLength = file.gcount();
            file.close();

            if (length == readLength)
            {
                utils::BufferReader br(fileData);

                res = swm_int::ParseModelInternal(br, model);
            }
        }

        return res;
    }

    SaveResult SaveSwm(std::filesystem::path filePath, const Model& model, SaveOptions options)
    {
        SaveResult res = SaveResult::FileIoError;

        std::ofstream file(filePath, std::ios::binary);

        if (file.is_open())
        {
            utils::BufferWriter bw(std::move(file));

            res = swm_int::WriteModelInternal(bw, model, options);
        }

        return res;
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
