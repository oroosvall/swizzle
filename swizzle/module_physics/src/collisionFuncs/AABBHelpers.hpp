
#pragma once

/* Include files */

#include <common/Common_new.hpp>
#include <physics/shapes/AABB.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace physics
{
    inline AABB AabbMinkowskiDifference(const AABB& s1, const AABB& s2)
    {
        AABB result{};
        result.mPos = s1.mPos - s2.mPos;
        result.mHalfSlab = s1.mHalfSlab + s2.mHalfSlab;
        return result;
    }

    inline void AabbGetMinMax(const AABB& aabb, glm::vec3& min, glm::vec3& max)
    {
        min = aabb.mPos - aabb.mHalfSlab;
        max = aabb.mPos + aabb.mHalfSlab;
    }
}