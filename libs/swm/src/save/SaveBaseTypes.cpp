
/* Include files */

#include "SaveBaseTypes.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swm_base
{
    swm::SaveResult SaveString(utils::BufferWriter& bw, const std::string& str)
    {
        swm::SaveResult res = swm::SaveResult::StringError;
        size_t size = str.size();
        if (size < 255u)
        {
            bw.write<U8>((U8)size);
            bw.writeArray<U8>((U8*)str.data(), size);
            res = swm::SaveResult::Success;
        }

        return res;
    }

}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
