
/* Include files */

#include "ShaderPropertyParser.hpp"
#include "ShaderProperty.hpp"

/* Defines */

/* Typedefs */

namespace vk::shader::_int
{
    typedef SwBool (*pfn_PropertyParse)(Property& out, const std::string& value);
}

/* Static Function Declaration */

namespace vk::shader::_int
{
    static std::vector<std::string> split(std::string s, std::string delimiter);
    static SwBool ParseBlendType(Property& out, const std::string& value);
    static SwBool ParseCullMode(Property& out, const std::string& value);
    static SwBool ParseBlendOp(Property& out, const std::string& value);

    static VkStencilOp GetStencilOp(std::string& str);
    static VkCompareOp GetCompareOp(std::string& str);
    static SwBool ParseStencilOp(Property& out, const std::string& value);
    static SwBool ParseStencilCompare(Property& out, const std::string& value);
    static SwBool ParseStencilMask(Property& out, const std::string& value);
    static SwBool ParseStencilEnable(Property& out, const std::string& value);
    static SwBool ParseDepthCompare(Property& out, const std::string& value);
    static SwBool ParseWriteMask(Property& out, const std::string& value);

} // namespace vk::shader::_int

/* Structs/Classes */

namespace vk::shader::_int
{
    struct BlendFactors
    {
        std::string mName;
        VkBlendFactor mFactor;
    };

    struct StencilOps
    {
        std::string mName;
        VkStencilOp mFactor;
    };

    struct CompareOps
    {
        std::string mName;
        VkCompareOp mCompare;
    };

    struct PropertyTypes
    {
        std::string mName;
        PropertyType mType;
        pfn_PropertyParse mParseFunc;
    };
} // namespace vk::shader::_int

/* Static Variables */

namespace vk::shader::_int
{
    BlendFactors blendFactors[] = {
        {"ONE", VkBlendFactor::VK_BLEND_FACTOR_ONE},
        {"ZERO", VkBlendFactor::VK_BLEND_FACTOR_ZERO},
        {"SRC_ALPHA", VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA},
        {"SRC_COLOR", VkBlendFactor::VK_BLEND_FACTOR_SRC_COLOR},
        {"DST_ALPHA", VkBlendFactor::VK_BLEND_FACTOR_DST_ALPHA},
        {"DST_COLOR", VkBlendFactor::VK_BLEND_FACTOR_DST_COLOR},
        {"ONE_MINUS_SRC_ALPHA", VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA},
        {"ONE_MINUS_SRC_COLOR", VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR},
        {"ONE_MINUS_DST_ALPHA", VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA},
        {"ONE_MINUS_DST_COLOR", VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR},
        {"ONE_MINUS_SRC_1_ALPHA", VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA},
        {"ONE_MINUS_SRC_1_COLOR", VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR},
        {"CONST_ALPHA", VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_ALPHA},
        {"CONST_COLOR", VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_COLOR},
    };

    StencilOps stencilOps[] = {
        {"OpKeep", VkStencilOp::VK_STENCIL_OP_KEEP},
        {"OpZero", VkStencilOp::VK_STENCIL_OP_ZERO},
        {"OpReplace", VkStencilOp::VK_STENCIL_OP_REPLACE},
        {"OpIncClamp", VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_CLAMP},
        {"OpDecClamp", VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_CLAMP},
        {"OpInvert", VkStencilOp::VK_STENCIL_OP_INVERT},
        {"OpIncWrap", VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_WRAP},
        {"OpIncWrap", VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_WRAP},
    };

    CompareOps compareOps[] = {
        {"OpNever", VkCompareOp::VK_COMPARE_OP_NEVER},
        {"OpLess", VkCompareOp::VK_COMPARE_OP_LESS},
        {"OpEq", VkCompareOp::VK_COMPARE_OP_EQUAL},
        {"OpLeEq", VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL},
        {"OpGreater", VkCompareOp::VK_COMPARE_OP_GREATER},
        {"OpNeq", VkCompareOp::VK_COMPARE_OP_NOT_EQUAL},
        {"OpGrEq", VkCompareOp::VK_COMPARE_OP_GREATER_OR_EQUAL},
        {"OpAlways", VkCompareOp::VK_COMPARE_OP_ALWAYS},
    };

    PropertyTypes propertyTypes[] = {
        {"sourceBlend", PropertyType::SourceBlend, ParseBlendType},
        {"destinationBlend", PropertyType::DestinationBlend, ParseBlendType},
        {"cullMode", PropertyType::CullMode, ParseCullMode},
        {"colorBlend", PropertyType::ColorBlendOp, ParseBlendOp},
        {"alphaBlend", PropertyType::AlphaBlendOp, ParseBlendOp},
        {"stencilOp", PropertyType::StencilOp, ParseStencilOp},
        {"stencilCompare", PropertyType::StencilCompare, ParseStencilCompare},
        {"stencilMask", PropertyType::StencilMask, ParseStencilMask},
        {"stencilEnable", PropertyType::StencilEnable, ParseStencilEnable},
        {"depthCompare", PropertyType::DepthCompare, ParseDepthCompare},
        {"writeMask", PropertyType::WriteMask, ParseWriteMask }
    };

} // namespace vk::shader::_int

/* Static Function Definition */

namespace vk::shader::_int
{

    static std::vector<std::string> split(std::string s, std::string delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
        {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }

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

    static VkStencilOp GetStencilOp(std::string& str)
    {
        for (const auto& factor : vk::shader::_int::stencilOps)
        {
            if (factor.mName == str)
            {
                return factor.mFactor;
            }
        }

        return VkStencilOp::VK_STENCIL_OP_KEEP;
    }

    static VkCompareOp GetCompareOp(std::string& str)
    {
        for (const auto& factor : vk::shader::_int::compareOps)
        {
            if (factor.mName == str)
            {
                return factor.mCompare;
            }
        }

        return VkCompareOp::VK_COMPARE_OP_LESS;
    }

    static SwBool ParseStencilOp(Property& out, const std::string& value)
    {
        std::vector<std::string> ops = split(value, ",");

        out.mStencilOp[0] = GetStencilOp(ops[0]);
        out.mStencilOp[1] = GetStencilOp(ops[1]);
        out.mStencilOp[2] = GetStencilOp(ops[2]);

        return true;
    }

    static SwBool ParseStencilCompare(Property& out, const std::string& value)
    {
        std::string val = value;
        out.mStencilCompare = GetCompareOp(val);

        return true;
    }

    static SwBool ParseStencilMask(Property& out, const std::string& value)
    {
        std::vector<std::string> val = split(value, ",");

        out.mStencilMasks[0] = std::stoul(val[0]);
        out.mStencilMasks[1] = std::stoul(val[1]);
        out.mStencilMasks[2] = std::stoul(val[2]);

        return true;
    }

    static SwBool ParseStencilEnable(Property& out, const std::string& value)
    {
        out.mEnableStencilWrite = (std::stoi(value) == 1);
        return true;
    }

    static SwBool ParseDepthCompare(Property& out, const std::string& value)
    {
        std::string val = value;
        out.mDepthCompare = GetCompareOp(val);

        return true;
    }

    static SwBool ParseWriteMask(Property& out, const std::string& value)
    {
        std::vector<std::string> val = split(value, ",");

        if (val.size() == 4)
        {
            val[0].erase(0, 1);
            val[3].pop_back();
            out.mWriteMask = std::stoul(val[0]) == 1 ? VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT : 0;
            out.mWriteMask |= std::stoul(val[1]) == 1 ? VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT : 0;
            out.mWriteMask |= std::stoul(val[2]) == 1 ? VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT : 0;
            out.mWriteMask |= std::stoul(val[3]) == 1 ? VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT : 0;
            return true;
        }

        return false;
    }
} // namespace vk::shader::_int

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
} // namespace vk::shader

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
