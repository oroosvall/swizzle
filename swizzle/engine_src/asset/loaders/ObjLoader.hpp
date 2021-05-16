
#ifndef OBJ_LOADER_HPP
#define OBJ_LOADER_HPP

/* Include files */

#include <common/Types.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "LoaderCommon.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */



/* Class Declaration */

/* Function Declaration */

namespace loader::obj
{
    SwBool loadObjFile(const SwChar* fileName, F32** vertexData, U32& numVertecies, U32** indexData, U32& numTriangles);
}

namespace swizzle
{
    struct Index
    {
        glm::ivec4 mVidx;
        glm::ivec4 mNidx;
        glm::ivec4 mUidx;
    };

    struct ObjData
    {
        std::vector<glm::vec3> mPositions;
        std::vector<glm::vec3> mNormals;
        std::vector<glm::vec2> mUvs;

        SwBool mHasVert;
        SwBool mHasNormal;
        SwBool mHasUv;

        std::vector<Index> mFaces;
    };

}

#endif