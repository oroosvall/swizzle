
/* Include files */

#include "CollisionFuncs.hpp"
#include "AABBHelpers.hpp"
#include "OOBBHelpers.hpp"

#include <cmath>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace physics
{
    SwBool SphereVsSphere(const Sphere& s1, const Sphere& s2)
    {
        const F32 distRadius = s1.mRadius + s2.mRadius;
        const F32 distPos = glm::distance(s1.mPos, s2.mPos);

        return distRadius > distPos;
    }

    RayHit SphereRaycast(const Sphere& moving, const glm::vec3& vel, const Sphere collider)
    {
        RayHit hit{};
        Sphere addedSphere = collider;
        addedSphere.mRadius += moving.mRadius;
        const F32 l = glm::length(vel);

        glm::vec3 movingToColliderDist = moving.mPos - collider.mPos;
        F32 b = glm::dot(movingToColliderDist, glm::normalize(vel));
        F32 c = glm::dot(movingToColliderDist, movingToColliderDist) - (addedSphere.mRadius * addedSphere.mRadius);

        if (c > 0.0f && b > 0.0f)
        {
            return hit;
        }

        F32 disc = (b * b) - c;
        if (disc < 0.0f)
        {
            return hit;
        }

        F32 t = (-b - sqrt(disc)) / l;

        if (t > 0.0f && t <= 1.0f)
        {
            hit.mHit = true;
            hit.mTime = t;
            hit.mPos = moving.mPos + hit.mTime * vel;
        }

        return hit;
    }

    SwBool PointInSphere(const glm::vec3& p, const Sphere& s)
    {
        const F32 x = p.x - s.mPos.x;
        const F32 y = p.y - s.mPos.y;
        const F32 z = p.z - s.mPos.z;
        const F32 r2 = s.mRadius * s.mRadius;

        F32 distSquared = (x * x) + (y * y) + (z * z);
        return distSquared < r2;
    }

    ResponseNormals SphereVsSphereResponseNormals(const Sphere& s1, const Sphere& s2)
    {
        const glm::vec3 norm = glm::normalize(s1.mPos - s2.mPos);
        return {norm, -norm};
    }

    SwBool AabbVsAabb(const AABB& s1, const AABB& s2)
    {
        AABB minkowskiDifference = AabbMinkowskiDifference(s1, s2);
        glm::vec3 min = {};
        glm::vec3 max = {};

        AabbGetMinMax(minkowskiDifference, min, max);

        return min.x <= 0.0f && max.x >= 0.0f && min.y <= 0.0f && max.y >= 0.0f && min.z <= 0.0f && max.z >= 0.0f;
    }

    RayHit AabbRaycast(const AABB& moving, const glm::vec3& vel, const AABB& collider)
    {
        RayHit hit{};
        AABB addedAABB = collider;
        addedAABB.mHalfSlab += moving.mHalfSlab;

        glm::vec3 min = {};
        glm::vec3 max = {};

        AabbGetMinMax(addedAABB, min, max);

        F32 lastEntry = -INFINITY;
        F32 firstExit = INFINITY;

        for (int i = 0; i < 3; ++i)
        {
            if (vel[i] != 0.0f)
            {
                F32 t1 = (min[i] - moving.mPos[i]) / vel[i];
                F32 t2 = (max[i] - moving.mPos[i]) / vel[i];

                lastEntry = fmaxf(lastEntry, fminf(t1, t2));
                firstExit = fminf(firstExit, fmaxf(t1, t2));
            }
            else if (moving.mPos[i] <= min[i] || moving.mPos[i] >= max[i])
            {
                return hit;
            }
        }

        if (firstExit > lastEntry && firstExit > 0.0f && lastEntry <= 1.0f)
        {
            hit.mPos = moving.mPos + vel * lastEntry;
            hit.mHit = true;
            hit.mTime = lastEntry;
        }

        return hit;
    }

    SwBool AabbVsSphere(const AABB& s1, const Sphere& s2)
    {
        glm::vec3 min = {};
        glm::vec3 max = {};
        AabbGetMinMax(s1, min, max);

        glm::vec3 p = glm::max(min, glm::min(s2.mPos, max));
        return PointInSphere(p, s2);
    }

    SwBool OobbVsOobb(const OOBB& s1, const OOBB& s2)
    {
        std::array<glm::vec3, 3> s1Normals{};
        std::array<glm::vec3, 3> s2Normals{};

        std::array<glm::vec3, 8> s1Vertices{};
        std::array<glm::vec3, 8> s2Vertices{};

        OobbGetNormals(s1, s1Normals);
        OobbGetNormals(s2, s2Normals);

        OobbGetVertices(s1, s1Vertices);
        OobbGetVertices(s2, s2Vertices);

        std::array<glm::vec3, 15> allAxis{};

        OobbGetAllAxis(s1Normals, s2Normals, allAxis);

        for (const auto& ax : allAxis)
        {
            if (AxisSeparated(s1Vertices, s2Vertices, ax))
            {
                return false;
            }
        }

        return true;
    }

    RayHit OobbRaycast(const OOBB& moving, const glm::vec3& vel, const OOBB& collider)
    {
        RayHit hit{};

        F32 tMin = 0.0f;
        F32 tMax = FLT_MAX;

        glm::vec3 oobbRayDist = collider.mPos - moving.mPos;

        OOBB added = collider;
        added.mHalfSlab += moving.mHalfSlab;

        glm::vec3 min = added.mHalfSlab;
        glm::vec3 max = -added.mHalfSlab;

        std::array<glm::vec3, 3> normals{};
        OobbGetNormals(added, normals);

        for (int i = 0; i < 3; ++i)
        {
            const auto& ax = normals[i];
            F32 e = glm::dot(ax, oobbRayDist);
            F32 f = glm::dot(glm::normalize(vel), ax);

            if (e > 0.0f && f > 0.0f)
            {
                return hit;
            }

            F32 t1 = (e + min[i]) / f;
            F32 t2 = (e + max[i]) / f;

            if (t1 > t2)
            {
                F32 w = t1;
                t1 = t2;
                t2 = w;
            }

            tMin = glm::max(tMin, t1);
            tMax = glm::min(tMax, t2);

            if(tMax < tMin)
            {
                return hit;
            }
        }

        hit.mHit = true;
        hit.mTime = tMin;
        hit.mPos = moving.mPos + vel * tMin;

        return hit;
    }

} // namespace physics

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
