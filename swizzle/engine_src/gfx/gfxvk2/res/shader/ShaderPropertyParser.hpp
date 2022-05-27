#ifndef SHADER_PROPERTY_PARSER_HPP
#define SHADER_PROPERTY_PARSER_HPP

/* Include files */

#include "ShaderProperty.hpp"

#include "../../backend/Vk.hpp"

#include <string>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace vk::shader
{
    VkBlendFactor GetBlendFactor(std::string& str);
    SwBool ReadProperty(const std::string& line, Property& outProperty, PropertyType& type);
}

#endif
