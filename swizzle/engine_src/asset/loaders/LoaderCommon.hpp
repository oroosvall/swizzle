#ifndef LOADER_COMMON_HPP
#define LOADER_COMMON_HPP

/* Include files */

#include <common/Types.hpp>
#include <glm/glm.hpp>
#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle
{
    struct ModelContents
    {
        SwBool mHasNormals;
        SwBool mHasUv;
        SwBool mHasColor;
        SwBool mHasSkinning;

        U8 mStride;
        std::vector<F32> mVertexData;
        std::vector<glm::ivec3> mFaces;

    };
}

namespace loader
{
    struct Vertex2D
    {
        F32 u;
        F32 v;
    };

    struct Vertex3D
    {
        F32 x;
        F32 y;
        F32 z;
    };

    struct Vertex
    {
        Vertex3D pos;
        Vertex3D norm;
        Vertex2D uv;
    };

    struct Index
    {
        U32 i1;
        U32 i2;
        U32 i3;
    };
}

/* Class Declaration */

/* Function Declaration */


#endif