
#pragma once

/* Include files */

#include <common/Common_new.hpp>
#include <glm/glm.hpp>

#include <physics/shapes/Sphere.hpp>
#include <physics/shapes/AABB.hpp>

/* Defines */

/* Typedefs/enums */

namespace physics
{
    enum class ShapeType
    {
        Sphere,
        AABB
    };
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace physics
{
    struct PhysicsShape
    {
        ShapeType mShape;
        union
        {
            Sphere mSphere;
            AABB mAABB;
        };
    };

    struct PhysicsObject
    {
        glm::vec3 mPos;
        glm::vec3 mVelocity;
        glm::vec3 mForce;
        F32 mMass;

        PhysicsShape* mShape;
    };

    struct StaticObject
    {
        glm::vec3 mPos;

        PhysicsShape* mShape;
    };
}

/* Class Declaration */

/* Function Declaration */