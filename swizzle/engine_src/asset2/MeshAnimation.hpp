#ifndef MESH_ANIMATION_HPP
#define MESH_ANIMATION_HPP

/* Include files */

#include <common/Common.hpp>

#include <swm/Swm.hpp>
#include <vector>
#include <glm/glm.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::asset2
{
    struct KeyFrame
    {
        std::vector<glm::mat4> mFrameData;
    };

    struct Animation
    {
        std::string mName;
        std::vector<KeyFrame> mKeyFrames;
    };
}

/* Class Declaration */

/* Function Declaration */

namespace swizzle::asset2
{
    void LoadAnimations(const swm::Model::Mesh& mesh, std::vector<Animation>& animationData);
}

#endif
