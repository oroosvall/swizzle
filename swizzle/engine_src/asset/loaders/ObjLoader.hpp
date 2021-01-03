
#pragma once

#include <swizzle/core/Types.hpp>
#include <glm/glm.hpp>
#include <vector>

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