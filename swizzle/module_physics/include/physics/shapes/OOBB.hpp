
#pragma once

/* Include files */

#include <common/Common_new.hpp>
#include <glm/glm.hpp>
#pragma warning(push)
#pragma warning(disable: 4201)
#include <glm/gtc/quaternion.hpp>
#pragma warning(pop)

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace physics
{
    struct OOBB
    {
        glm::vec3 mPos;
        glm::quat mRot;
        glm::vec3 mHalfSlab;
    };
}

/* Class Declaration */

/* Function Declaration */