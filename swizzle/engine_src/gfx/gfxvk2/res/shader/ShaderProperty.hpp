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
        StencilOp,
        StencilCompare,
        StencilMask,
        StencilEnable,
        DepthCompare,
    };

    enum class ShaderModuleType
    {
        ShaderModuleType_Vertex,
        ShaderModuleType_TessellationControl,
        ShaderModuleType_TessellationEvaluate,
        ShaderModuleType_Geometry,
        ShaderModuleType_Fragment,
        ShaderModuleType_Compute,
        ShaderModuleType_Task,
        ShaderModuleType_Mesh
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
            VkStencilOp mStencilOp[3];
            VkCompareOp mStencilCompare;
            U32 mStencilMasks[3];
            SwBool mEnableStencilWrite;
            VkCompareOp mDepthCompare;
        };
    };
}

/* Class Declaration */

/* Function Declaration */

#endif
