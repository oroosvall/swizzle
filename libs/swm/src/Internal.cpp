
/* Include files */

#include "Internal.hpp"
#include "VersionSelect.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swm_int
{
    SwBool CheckMagic(const Magic* magic)
    {
        SwBool ok = false;
        if (magic != nullptr)
        {
            if (
                (magic->mMagic[0] == 'S') &&
                (magic->mMagic[1] == 'W') &&
                (magic->mMagic[2] == 'M') &&
                (magic->mMagic[3] == ' '))
            {
                ok = true;
            }

        }
        return ok;
    }

    swm::LoadResult ParseModelInternal(utils::BufferReader& br, swm::Model& model)
    {
        swm::LoadResult res = swm::LoadResult::InvalidVersion;

        swm_int::Magic* mag = br.read<swm_int::Magic>();

        if (swm_int::CheckMagic(mag))
        {
            Version* ver = br.read<Version>();

            if (ver != nullptr)
            {
                swm_ver::LoadFn loadFunc = swm_ver::FindLoader(ver->mMajor);

                if (loadFunc)
                {
                    res = loadFunc(ver->mMinor, br, model);
                }
                else
                {
                    // version not valid
                }
            }
            else
            {
                res = swm::LoadResult::SizeError;
            }
        }
        else
        {
            res = swm::LoadResult::InvalidHeader;
        }

        return res;
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
