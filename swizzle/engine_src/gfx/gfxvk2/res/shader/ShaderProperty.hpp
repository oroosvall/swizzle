#ifndef SHADER_PROPERTY_HPP
#define SHADER_PROPERTY_HPP

/* Include files */

#include "../../backend/Vk.hpp"

/* Defines */

/* Typedefs/enums */

namespace vk::shader
{
    enum class PropertyType
    {
        SourceBlend,
        DestinationBlend,
        CullMode,
        ColorBlendOp,
        AlphaBlendOp,
    };

    enum class ShaderModuleType
    {
        ShaderModuleType_Vertex,
        ShaderModuleType_Geometry,
        ShaderModuleType_Fragment
    };
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace vk::shader
{
    struct Property
    {
        union
        {
            VkBlendFactor mBlendFactor[2];
            VkCullModeFlagBits mCullMode;
            VkBlendOp mColorBlendOp;
            VkBlendOp mAlphaBlendOp;
        };
    };
}

/* Class Declaration */

/* Function Declaration */

#endif
