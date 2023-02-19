
#include <physics/Physics.hpp>

#include "collisionFuncs/CollisionFuncs.hpp"

namespace physics
{
    AABB simulateMove(const AABB& aabb, const glm::vec3& vel, const OOBB& obb)
    {
        OOBB fromAabb{};
        fromAabb.mHalfSlab = aabb.mHalfSlab;
        fromAabb.mPos = aabb.mPos;
        fromAabb.mRot = {1.0f, 0.0f, 0.0f, 0.0f};

        RayHit h = OobbRaycast(fromAabb, vel, obb);

        AABB copy = aabb;
        if (h.mHit)
        {
            copy.mPos = h.mPos;
        }

        return copy;
    }

    AABB simulateMove(const AABB& aabb, const glm::vec3& vel, const AABB& aabb2)
    {
        RayHit h = AabbRaycast(aabb, vel, aabb2);

        AABB copy = aabb;
        copy.mPos = h.mPos;

        return copy;
    }
}