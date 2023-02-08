
#pragma once

/* Include files */

#include <common/Common_new.hpp>
#include <physics/shapes/Sphere.hpp>
#include <physics/shapes/AABB.hpp>
#include <physics/shapes/OOBB.hpp>
#include "CollisionResponse.hpp"
#include "RayHit.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace physics
{
    SwBool SphereVsSphere(const Sphere& s1, const Sphere& s2);
    RayHit SphereRaycast(const Sphere& moving, const glm::vec3& vel, const Sphere collider);
    SwBool PointInSphere(const glm::vec3& p, const Sphere& s);

    ResponseNormals SphereVsSphereResponseNormals(const Sphere& s1, const Sphere& s2);

    SwBool AabbVsAabb(const AABB& s1, const AABB& s2);
    RayHit AabbRaycast(const AABB& moving, const glm::vec3& vel, const AABB& collider);

    SwBool AabbVsSphere(const AABB& s1, const Sphere& s2);

    SwBool OobbVsOobb(const OOBB& s1, const OOBB& s2);

    RayHit OobbRaycast(const OOBB& moving, const glm::vec3& vel, const OOBB& collider);
}