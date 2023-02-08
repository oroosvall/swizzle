
#pragma once

/* Include files */

#include <common/Common_new.hpp>
#include <glm/glm.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace physics
{
    struct RayHit
    {
        SwBool mHit;
        F32 mTime;
        glm::vec3 mPos;
    };
}

/* Class Declaration */

/* Function Declaration */
