#ifndef PHYSICS_HPP
#define PHYSICS_HPP

/* Include files */

#include <physics/shapes/AABB.hpp>
#include <physics/shapes/OOBB.hpp>
#include <physics/shapes/Sphere.hpp>

#include <glm/glm.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace physics
{
    AABB simulateMove(const AABB& aabb, const glm::vec3& vel, const OOBB& obb);
    AABB simulateMove(const AABB& aabb, const glm::vec3& vel, const AABB& aabb2);

    AABB fromOOBB(const OOBB& obb);
}

#endif
