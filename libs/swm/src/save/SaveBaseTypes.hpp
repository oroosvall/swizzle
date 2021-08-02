#ifndef SAVE_BASE_TYPES_HPP
#define SAVE_BASE_TYPES_HPP

/* Include files */

#include <common/Common.hpp>
#include <swm/Swm.hpp>
#include <utils/BufferWriter.hpp>
#include <string>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace swm_base
{
    swm::SaveResult SaveString(utils::BufferWriter& bw, const std::string& str);
}

#endif