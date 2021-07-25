
/* Include files */

#include "VersionSelect.hpp"
#include "load/LoadV1.hpp"
#include "load/LoadV2.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */
namespace swm_ver
{
    LoadFn FindLoader(U16 majorVer)
    {
        LoadFn fn = nullptr;
        if (majorVer == 1u)
        {
            fn = &swm_v1::LoadV1;
        }
        else if(majorVer == 2u)
        {
            fn = &swm_v2::LoadV2;
        }
        else
        {
            // unknown version
        }
        return fn;
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
