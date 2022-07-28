
/* Include files */

#include <swm/Swm.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

namespace swm
{
    const SwChar* gResultStrings[] = {
        /* Result::Success              */ "Success",
        /* Result::ErrFileIo            */ "File IO read error",
        /* Result::ErrMagic             */ "Incorrect magic",
        /* Result::ErrVersion           */ "File version not supported",
        /* Result::ErrDataSizeMissmatch */ "Size missmatch between vertices and optional data",
        /* Result::ErrToManyAttributes  */ "To many attributes specified for the current channel type",
    };
}

/* Static Function Definition */

/* Function Definition */

namespace swm
{
    const SwChar* GetErrorDescription(Result result)
    {
        const U32 idx = (U32)result;
        const SwChar* resultString = "Unknown result";

        if (idx < COUNT_OF(gResultStrings))
        {
            resultString = gResultStrings[idx];
        }

        return resultString;
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
