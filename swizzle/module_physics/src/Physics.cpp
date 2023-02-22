
#include <physics/Physics.hpp>

#include "collisionFuncs/CollisionFuncs.hpp"
#include "collisionFuncs/OOBBHelpers.hpp"

namespace physics
{
    AABB simulateMove(const AABB& aabb, const glm::vec3& vel, const OOBB& obb)
    {
        OOBB fromAabb{};
        fromAabb.mHalfSlab = aabb.mHalfSlab;
        fromAabb.mPos = aabb.mPos;
        fromAabb.mRot = {1.0f, 0.0f, 0.0f, 0.0f};

        //RayHit h = OobbRaycast(fromAabb, vel, obb);

        //auto tmp = obb;
        //tmp.mHalfSlab += aabb.mHalfSlab;
        //AABB fromObb = OobbGetAABB(tmp);
        //RayHit h = AabbRaycast(aabb, vel, fromObb);

        //OOBB fromAabb2{};
        //fromAabb2.mHalfSlab = fromObb.mHalfSlab;// +aabb.mHalfSlab;
        //fromAabb2.mPos = fromObb.mPos;
        //fromAabb2.mRot = obb.mRot;

        RayHit h = OobbRaycast(fromAabb, vel, obb);

        AABB copy = aabb;
        if (h.mHit)
        {
            copy.mPos = h.mPos;
        }
        else
        {
            copy.mPos += vel;
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

    AABB fromOOBB(const OOBB& obb)
    {
        return OobbGetAABB(obb);
    }
}