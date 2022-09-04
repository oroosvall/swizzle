
/* Include files */

#include "AssetUtils.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::asset2
{
    U64 AttributeSize(swizzle::asset2::AttributeTypes attrib)
    {
        U64 size = 0u;
        switch (attrib)
        {
        case swizzle::asset2::AttributeTypes::VertexPosition:
        case swizzle::asset2::AttributeTypes::NormalVector:
            size = 3u;
            break;
        case swizzle::asset2::AttributeTypes::UvCoordinates:
            size = 2u;
            break;
        case swizzle::asset2::AttributeTypes::BoneIndices:
        case swizzle::asset2::AttributeTypes::BoneWeights:
        case swizzle::asset2::AttributeTypes::VertexColor:
            size = 4u;
            break;
        default:
            break;
        }
        return size * sizeof(float);
    }

    const SwChar* GetAttribName(swizzle::asset2::AttributeTypes attrib)
    {
        switch (attrib)
        {
        case swizzle::asset2::AttributeTypes::VertexPosition:
            return "VertexPosition";
        case swizzle::asset2::AttributeTypes::NormalVector:
            return "NormalVector";
        case swizzle::asset2::AttributeTypes::UvCoordinates:
            return "UvCoordinates";
        case swizzle::asset2::AttributeTypes::BoneIndices:
            return "BoneIndices";
        case swizzle::asset2::AttributeTypes::BoneWeights:
            return "BoneWeights";
        case swizzle::asset2::AttributeTypes::VertexColor:
            return "VertexColor";
        default:
            return "Undefined";
        }
    }

}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
