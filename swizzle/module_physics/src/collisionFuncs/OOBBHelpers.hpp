
#pragma once

/* Include files */

#include <common/Common_new.hpp>
#include <physics/shapes/OOBB.hpp>
#include <physics/shapes/AABB.hpp>

#include <array>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace physics
{
    inline void OobbGetVertices(const OOBB& oobb, std::array<glm::vec3, 8>& outVertices)
    {
        glm::vec3 min = oobb.mHalfSlab;
        glm::vec3 max = -oobb.mHalfSlab;

        outVertices[0] = oobb.mPos + oobb.mRot * min;
        outVertices[1] = oobb.mPos + oobb.mRot * glm::vec3(min.x, min.y, max.z);
        outVertices[2] = oobb.mPos + oobb.mRot * glm::vec3(min.x, max.y, min.z);
        outVertices[3] = oobb.mPos + oobb.mRot * glm::vec3(min.x, max.y, max.z);
        outVertices[4] = oobb.mPos + oobb.mRot * glm::vec3(max.x, min.y, min.z);
        outVertices[5] = oobb.mPos + oobb.mRot * glm::vec3(max.x, min.y, max.z);
        outVertices[6] = oobb.mPos + oobb.mRot * glm::vec3(max.x, max.y, min.z);
        outVertices[7] = oobb.mPos + oobb.mRot * max;
    }

    inline AABB OobbGetAABB(const OOBB& oobb)
    {
        glm::vec3 min = oobb.mHalfSlab;
        glm::vec3 max = -oobb.mHalfSlab;

        glm::vec3 halfSlab {};

        halfSlab = glm::max(halfSlab, min);
        halfSlab = glm::max(halfSlab, glm::vec3(min.x, min.y, max.z));
        halfSlab = glm::max(halfSlab, glm::vec3(min.x, max.y, min.z));
        halfSlab = glm::max(halfSlab, glm::vec3(min.x, max.y, max.z));
        halfSlab = glm::max(halfSlab, glm::vec3(max.x, min.y, min.z));
        halfSlab = glm::max(halfSlab, glm::vec3(max.x, min.y, max.z));
        halfSlab = glm::max(halfSlab, glm::vec3(max.x, max.y, min.z));
        halfSlab = glm::max(halfSlab, max);

        AABB aabb{};
        aabb.mHalfSlab = halfSlab;
        aabb.mPos = oobb.mPos;

        return aabb;
    }

    inline void OobbGetNormals(const OOBB& oobb, std::array<glm::vec3, 3>& outNormals)
    {
        outNormals[0] = glm::normalize(oobb.mRot * glm::vec3(1.0f, 0.0f, 0.0f));
        outNormals[1] = glm::normalize(oobb.mRot * glm::vec3(0.0f, 1.0f, 0.0f));
        outNormals[2] = glm::normalize(oobb.mRot * glm::vec3(0.0f, 0.0f, 1.0f));
    }

    inline void OobbGetAllAxis(const std::array<glm::vec3, 3>& s1, const std::array<glm::vec3, 3>& s2, std::array<glm::vec3, 15>& allAxis)
    {
        allAxis[0] = s1[0];
        allAxis[1] = s1[1];
        allAxis[2] = s1[2];

        allAxis[3] = s2[0];
        allAxis[4] = s2[1];
        allAxis[5] = s2[2];

        allAxis[6] = glm::cross(s1[0], s2[0]);
        allAxis[7] = glm::cross(s1[0], s2[1]);
        allAxis[8] = glm::cross(s1[0], s2[2]);

        allAxis[9] = glm::cross(s1[1], s2[0]);
        allAxis[10] = glm::cross(s1[1], s2[1]);
        allAxis[11] = glm::cross(s1[1], s2[2]);

        allAxis[12] = glm::cross(s1[2], s2[0]);
        allAxis[13] = glm::cross(s1[2], s2[1]);
        allAxis[14] = glm::cross(s1[2], s2[2]);
    }

    inline SwBool AxisSeparated(const std::array<glm::vec3, 8>& vertA, const std::array<glm::vec3, 8>& vertB, const glm::vec3 axis)
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
            bMax = glm::max(aDist, bMax);
        }

        F32 longSpan = glm::max(aMax, bMax) - glm::min(aMin, bMin);
        F32 sumSpan = (aMax - aMin) + (bMax - bMin);
        return longSpan >= sumSpan;
    }
}