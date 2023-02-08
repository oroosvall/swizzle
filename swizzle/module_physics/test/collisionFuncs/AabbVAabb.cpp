
/* Include files */

#include <gtest/gtest.h>

#include "collisionFuncs/CollisionFuncs.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace physicsTests
{
    TEST(Aabb, overlaps)
    {
        physics::AABB s1{ {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}};
        physics::AABB s2{ {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f} };

        auto collides = physics::AabbVsAabb(s1, s2);
        EXPECT_TRUE(collides);
    }

    TEST(Aabb, noOverlap)
    {
        physics::AABB s1{ {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };
        physics::AABB s2{ {3.0f, 3.0f, 3.0f}, {1.0f, 1.0f, 1.0f} };

        auto collides = physics::AabbVsAabb(s1, s2);
        EXPECT_FALSE(collides);
    }

    TEST(Aabb, movingCollide)
    {
        physics::AABB obstacle{ {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };
        physics::AABB moving{ {-3.0f, -3.0f, -3.0f}, {1.0f, 1.0f, 1.0f} };

        glm::vec3 vel = { 6.0f, 6.0f, 6.0f };

        auto collides = physics::AabbVsAabb(obstacle, moving);
        EXPECT_FALSE(collides);

        physics::RayHit hit = physics::AabbRaycast(moving, vel, obstacle);
        EXPECT_TRUE(hit.mHit);
    }

    TEST(Aabb, movingNoCollide)
    {
        physics::AABB obstacle{ {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };
        physics::AABB moving{ {-3.0f, -3.0f, -3.0f}, {1.0f, 1.0f, 1.0f} };

        glm::vec3 vel = { 6.0f, 0.0f, 0.0f };

        auto collides = physics::AabbVsAabb(obstacle, moving);
        EXPECT_FALSE(collides);

        physics::RayHit hit = physics::AabbRaycast(moving, vel, obstacle);
        EXPECT_FALSE(hit.mHit);
    }

    TEST(Aabb, DISABLED_responseNormal)
    {
        //physics::Sphere s1{ {0.0f, 0.0f, 0.0f}, 1.0f };
        //physics::Sphere s2{ {1.0f, 0.0f, 0.0f}, 1.0f };

        //auto norms = physics::SphereVsSphereResponseNormals(s1, s2);

        //ExpectVec3FltEq(norms.mShape1ResponseNormal, { -1.0f, 0.0f, 0.0f });
        //ExpectVec3FltEq(norms.mShape2ResponseNormal, { 1.0f, 0.0f, 0.0f });
    }

    TEST(Aabb, SphereInAabb_1)
    {
        physics::AABB aabb{ {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };
        physics::Sphere sphere{ {0.0f, 0.0f, 0.0f}, 1.0f };

        EXPECT_TRUE(physics::AabbVsSphere(aabb, sphere));
    }

    TEST(Aabb, SphereInAabb_2)
    {
        physics::AABB aabb{ {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };
        physics::Sphere sphere{ {1.5f, 0.0f, 0.0f}, 1.0f };

        EXPECT_TRUE(physics::AabbVsSphere(aabb, sphere));
    }

    TEST(Aabb, SphereInAabb_3)
    {
        physics::AABB aabb{ {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };
        physics::Sphere sphere{ {0.0f, 1.5f, 0.0f}, 1.0f };

        EXPECT_TRUE(physics::AabbVsSphere(aabb, sphere));
    }

    TEST(Aabb, SphereInAabb_4)
    {
        physics::AABB aabb{ {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };
        physics::Sphere sphere{ {0.0f, 0.0f, 1.5f}, 1.0f };

        EXPECT_TRUE(physics::AabbVsSphere(aabb, sphere));
    }

    TEST(Aabb, SphereOutsieAabb_1)
    {
        physics::AABB aabb{ {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };
        physics::Sphere sphere{ {2.0f, 2.0f, 2.0f}, 1.0f };

        EXPECT_FALSE(physics::AabbVsSphere(aabb, sphere));
    }

    TEST(Aabb, SphereOutsieAabb_2)
    {
        physics::AABB aabb{ {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };
        physics::Sphere sphere{ {5.0f, 5.0f, 5.0f}, 1.0f };

        EXPECT_FALSE(physics::AabbVsSphere(aabb, sphere));
    }

} // namespace physicsTests

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
