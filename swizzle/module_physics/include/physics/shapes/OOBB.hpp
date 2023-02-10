
#pragma once

/* Include files */

#include <common/Common_new.hpp>
#include <glm/glm.hpp>
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4201)
#endif
#include <glm/gtc/quaternion.hpp>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

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