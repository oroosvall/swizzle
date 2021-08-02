#ifndef SWM_INTERNAL_HPP
#define SWM_INTERNAL_HPP

/* Include files */

#include <swm/Swm.hpp>

#include <common/Common.hpp>
#include <utils/BufferReader.hpp>
#include <utils/BufferWriter.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swm_int
{
    struct Version
    {
        U16 mMajor;
        U16 mMinor;
    };

    struct Magic
    {
        SwChar mMagic[4] = { 'S', 'W', 'M', ' ' };
    };
}

/* Class Declaration */

/* Function Declaration */

namespace swm_int
{
    SwBool CheckMagic(const Magic* ver);

    swm::LoadResult ParseModelInternal(utils::BufferReader& br, swm::Model& model);

    swm::SaveResult WriteModelInternal(utils::BufferWriter& bw, const swm::Model& model, const swm::SaveOptions& options);

}

#endif