
#pragma once

#include <common/Types.hpp>
#include <glm/glm.hpp>
#include <vector>

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