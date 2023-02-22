#pragma once

/* Include files */

#include <common/Common_new.hpp>
#include <glm/glm.hpp>

#include <array>
#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace physics
{
    inline SwBool IsAxisSeparated(const std::array<glm::vec3, 8>& vertA, const std::array<glm::vec3, 8>& vertB, const glm::vec3 axis)
    {
        if (axis.length() == 0)
        {
            return false;
        }

        F32 aMin = FLT_MAX;
        F32 bMin = FLT_MAX;
        F32 aMax = FLT_MIN;
        F32 bMax = FLT_MIN;

        for (S32 i = 0; i < 8; ++i)
        {
            F32 aDist = glm::dot(vertA[i], axis);

            aMin = glm::min(aDist, aMin);
            aMax = glm::max(aDist, aMax);

            F32 bDist = glm::dot(vertB[i], axis);
            bMin = glm::min(bDist, bMin);
            bMax = glm::max(bDist, bMax);
        }

        F32 longSpan = glm::max(aMax, bMax) - glm::min(aMin, bMin);
        F32 sumSpan = (aMax - aMin) + (bMax - bMin);
        return longSpan >= sumSpan;
    }

    inline SwBool IsAxisSeparatedVec(const std::vector<glm::vec3>& vertA, const std::vector<glm::vec3>& vertB, const glm::vec3 axis)
    {
        if (axis.length() == 0)
        {
            return false;
        }

        F32 aMin = FLT_MAX;
        F32 bMin = FLT_MAX;
        F32 aMax = FLT_MIN;
        F32 bMax = FLT_MIN;

        for (S32 i = 0; i < vertA.size(); ++i)
        {
            F32 aDist = glm::dot(vertA[i], axis);

            aMin = glm::min(aDist, aMin);
            aMax = glm::max(aDist, aMax);
        }

        for (S32 i = 0; i < vertB.size(); ++i)
        {
            F32 bDist = glm::dot(vertB[i], axis);
            bMin = glm::min(bDist, bMin);
            bMax = glm::max(bDist, bMax);
        }

        F32 longSpan = glm::max(aMax, bMax) - glm::min(aMin, bMin);
        F32 sumSpan = (aMax - aMin) + (bMax - bMin);
        return longSpan >= sumSpan;
    }
}