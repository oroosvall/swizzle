
/* Include files */

#include "LoadBaseTypes.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swm_base
{
    SwBool ResultIsOk(swm::LoadResult result)
    {
        return (result == swm::LoadResult::Success);
    }

    swm::LoadResult LoadString(utils::BufferReader& br, std::string& str)
    {
        swm::LoadResult res = swm::LoadResult::StringError;

        U8 len = 0u;
        if (br.readNum<U8>(len))
        {
            U8* strData = br.readArrayU32<U8>(len);
            if (strData)
            {
                str = std::string((char*)strData, len);
                res = swm::LoadResult::Success;
            }
        }

        return res;
    }

}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
