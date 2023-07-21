
/* Include files */

#include "FileUtils.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::core
{
    std::string getPathFromFileName(const std::string& fileName)
    {
        std::size_t found = fileName.find_last_of("/\\");
        return fileName.substr(0, found + 1);
    }

    std::vector<std::string> getPaths(const std::string& s)
    {
        std::string delimiter = "/\\";
        size_t startPos = 0ull;
        size_t endPos = 0ull;
        const size_t delim_len = 1ull;
        std::string token;
        std::vector<std::string> res;

        while ((endPos = s.find_first_of(delimiter, startPos)) != std::string::npos)
        {
            token = s.substr(startPos, endPos - startPos);
            startPos = endPos + delim_len;
            if (!token.empty())
            {
                res.push_back(token);
            }
        }
        std::string newS = s.substr(startPos);
        if (!newS.empty())
        {
            res.push_back(newS);
        }
        return res;
    }
} // namespace swizzle::core

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
