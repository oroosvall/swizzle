#ifndef LOADER_COMMON_HPP
#define LOADER_COMMON_HPP

/* Include files */

#include <common/Common.hpp>
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

    struct Int4D
    {
        U32 i1;
        U32 i2;
        U32 i3;
        U32 i4;
    };

    struct Weight4D
    {
        F32 w1;
        F32 w2;
        F32 w3;
        F32 w4;
    };

    struct Vertex
    {
        Vertex3D pos;
        Vertex3D norm;
        Vertex2D uv;
    };

    struct VertexAnim
    {
        Vertex3D pos;
        Vertex3D norm;
        Vertex2D uv;
        Int4D idx;
        Weight4D wgt;
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