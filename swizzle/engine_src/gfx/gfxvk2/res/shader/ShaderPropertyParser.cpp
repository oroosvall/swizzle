
/* Include files */

#include "ShaderProperty.hpp"
#include "ShaderPropertyParser.hpp"

/* Defines */

/* Typedefs */

namespace vk::shader::_int
{
    typedef SwBool(*pfn_PropertyParse)(Property& out, const std::string& value);
}

/* Static Function Declaration */

namespace vk::shader::_int
{
    static SwBool ParseBlendType(Property& out, const std::string& value);
    static SwBool ParseCullMode(Property& out, const std::string& value);
    static SwBool ParseBlendOp(Property& out, const std::string& value);
}

/* Structs/Classes */

namespace vk::shader::_int
{
    struct BlendFactors
    {
        std::string mName;
        VkBlendFactor mFactor;
    };

    struct PropertyTypes
    {
        std::string mName;
        PropertyType mType;
        pfn_PropertyParse mParseFunc;
    };
}

/* Static Variables */

namespace vk::shader::_int
{
    BlendFactors blendFactors[] = {
        { "ONE", VkBlendFactor::VK_BLEND_FACTOR_ONE },
        { "ZERO", VkBlendFactor::VK_BLEND_FACTOR_ZERO },
        { "SRC_ALPHA", VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA },
        { "SRC_COLOR", VkBlendFactor::VK_BLEND_FACTOR_SRC_COLOR },
        { "DST_ALPHA", VkBlendFactor::VK_BLEND_FACTOR_DST_ALPHA },
        { "DST_COLOR", VkBlendFactor::VK_BLEND_FACTOR_DST_COLOR },
        { "ONE_MINUS_SRC_ALPHA", VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA },
        { "ONE_MINUS_SRC_COLOR", VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR },
        { "ONE_MINUS_DST_ALPHA", VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA },
        { "ONE_MINUS_DST_COLOR", VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR },
        { "ONE_MINUS_SRC_1_ALPHA", VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA },
        { "ONE_MINUS_SRC_1_COLOR", VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR },
        { "CONST_ALPHA", VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_ALPHA },
        { "CONST_COLOR", VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_COLOR },
    };

    PropertyTypes propertyTypes[] = {
        { "sourceBlend", PropertyType::SourceBlend, ParseBlendType },
        { "destinationBlend", PropertyType::DestinationBlend, ParseBlendType },
        { "cullMode", PropertyType::CullMode, ParseCullMode },
        { "colorBlend", PropertyType::ColorBlendOp, ParseBlendOp },
        { "alphaBlend", PropertyType::AlphaBlendOp, ParseBlendOp },
    };

}

/* Static Function Definition */

namespace vk::shader::_int
{
    static SwBool ParseBlendType(Property& out, const std::string& value)
    {
        out = {};

        auto index2 = value.find(",");
        auto first = value.substr(1, index2 - 1);
        auto second = value.substr(index2 + 1);
        second.pop_back();

        out.mBlendFactor[0] = shader::GetBlendFactor(first);
        out.mBlendFactor[1] = shader::GetBlendFactor(second);

        return true;
    }

    static SwBool ParseCullMode(Property& out, const std::string& value)
    {
        SwBool ok = true;
        if (value == "CULL_NONE")
        {
            out.mCullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
        }
        else if (value == "CULL_FRONT")
        {
            out.mCullMode = VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
        }
        else if (value == "CULL_BACK")
        {
            out.mCullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
        }
        else
        {
            ok = false;
        }

        return ok;
    }

    static SwBool ParseBlendOp(Property& out, const std::string& value)
    {
        SwBool ok = true;
        if (value == "MULTIPLY")
        {
            out.mAlphaBlendOp = VkBlendOp::VK_BLEND_OP_MULTIPLY_EXT;
        }
        else if (value == "ADD")
        {
            out.mAlphaBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
        }
        else
        {
            ok = false;
        }
        return ok;
    }
}

/* Function Definition */

namespace vk::shader
{
    VkBlendFactor GetBlendFactor(std::string& str)
    {
        for (const auto& factor : vk::shader::_int::blendFactors)
        {
            if (factor.mName == str)
            {
                return factor.mFactor;
            }
        }

        return VkBlendFactor::VK_BLEND_FACTOR_ONE;
    }

    SwBool ReadProperty(const std::string& line, Property& outProperty, PropertyType& type)
    {
        SwBool ok = false;
        auto index = line.find('=');
        auto propertyName = line.substr(0, index);
        auto value = line.substr(index + 1);

        for (const auto& prop : vk::shader::_int::propertyTypes)
        {
            if (prop.mName == propertyName)
            {
                if (!prop.mParseFunc)
                {
                    LOG_CRITICAL("Property does not have a parse function");
                    return false;
                }
                ok = prop.mParseFunc(outProperty, value);
                type = prop.mType;
                break;
            }
        }

        return ok;
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
