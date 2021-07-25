#ifndef VERSION_SELECT_HPP
#define VERSION_SELECT_HPP

/* Include files */

#include <swm/Swm.hpp>
#include <utils/BufferReader.hpp>

/* Defines */

/* Typedefs/enums */

namespace swm_ver
{
    typedef swm::LoadResult(*LoadFn)(U16 minor, utils::BufferReader& br, swm::Model& model);
}
/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace swm_ver
{
    LoadFn FindLoader(U16 majorVer);
}

#endif