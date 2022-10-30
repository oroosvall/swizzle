
/* Include files */

#include "helpers/FileReader.hpp"
#include "helpers/FileWriter.hpp"
#include <swm/Swm.hpp>

#include "Load.hpp"
#include "Save.hpp"

#include <fstream>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swm
{
    Result LoadSwm(std::filesystem::path filePath, Model& model)
    {
        Result res = Result::ErrFileIo;

        std::ifstream file(filePath, std::ios::binary);
        if (file.is_open())
        {
            file::FileReader fr(file);
            res = load::LoadModel(fr, model);
        }

        return res;
    }

    Result SaveSwm(std::filesystem::path filePath, const Model& model, options::SaveOptions options)
    {
        Result res = Result::ErrFileIo;

        std::ofstream file(filePath, std::ios::binary);
        if (file.is_open())
        {
            file::FileWriter fw(file);
            res = save::SaveModel(fw, model, options);
        }

        return res;
    }
} // namespace swm

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
