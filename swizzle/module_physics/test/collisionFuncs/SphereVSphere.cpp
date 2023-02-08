
/* Include files */

#include <gtest/gtest.h>

#include "collisionFuncs/CollisionFuncs.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

static void ExpectVec3FltEq(glm::vec3 v1, glm::vec3 v2);

/* Static Variables */

/* Static Function Definition */

static void ExpectVec3FltEq(glm::vec3 v1, glm::vec3 v2)
{
    EXPECT_FLOAT_EQ(v1.x, v2.x);
    EXPECT_FLOAT_EQ(v1.y, v2.y);
    EXPECT_FLOAT_EQ(v1.z, v2.z);
}

/* Function Definition */

namespace physicsTests
{
    TEST(Sphere, overlaps)
    {
        physics::Sphere s1{{0.0f, 0.0f, 0.0f}, 1.0f};
        physics::Sphere s2{{1.0f, 0.0f, 0.0f}, 1.0f};

        auto resp = physics::SphereVsSphere(s1, s2);

        EXPECT_TRUE(resp);
    }

    TEST(Sphere, noOverlap)
    {
        physics::Sphere s1{{0.0f, 0.0f, 0.0f}, 1.0f};
        physics::Sphere s2{{3.0f, 0.0f, 0.0f}, 1.0f};

        auto resp = physics::SphereVsSphere(s1, s2);
        EXPECT_FALSE(resp);
    }

    TEST(Sphere, MovingSphere_still)
    {
        physics::Sphere s1{{0.0f, 0.0f, 0.0f}, 1.0f};
        physics::Sphere s2{{3.0f, 0.0f, 0.0f}, 1.0f};

        auto resp = physics::SphereRaycast(s1, {}, s2);
        EXPECT_FALSE(resp.mHit);
    }

    TEST(Sphere, MovingSphere_miss1)
    {
        physics::Sphere s1{ {0.0f, 0.0f, 0.0f}, 1.0f };
        physics::Sphere s2{ {3.0f, 0.0f, 0.0f}, 1.0f };

        auto resp = physics::SphereRaycast(s1, { -3.0f, 0.0f, 0.0f }, s2);
        EXPECT_FALSE(resp.mHit);
    }

    TEST(Sphere, MovingSphere_miss2)
    {
        physics::Sphere s1{ {0.0f, 0.0f, 0.0f}, 1.0f };
        physics::Sphere s2{ {3.0f, 0.0f, 0.0f}, 1.0f };

        auto resp = physics::SphereRaycast(s1, { 3.0f, 3.0f, 0.0f }, s2);
        EXPECT_FALSE(resp.mHit);
    }

    TEST(Sphere, MovingSphere_Towards1)
    {
        physics::Sphere s1{{0.0f, 0.0f, 0.0f}, 1.0f};
        physics::Sphere s2{{5.0f, 0.0f, 0.0f}, 1.0f};

        auto resp = physics::SphereRaycast(s1, {3.0f, 0.0f, 0.0f}, s2);
        EXPECT_TRUE(resp.mHit);
    }

    TEST(Sphere, MovingSphere_Towards2)
    {
        physics::Sphere s1{ {0.0f, 0.0f, 0.0f}, 1.0f };
        physics::Sphere s2{ {10.0f, 0.0f, 0.0f}, 1.0f };

        auto resp = physics::SphereRaycast(s1, { 10.0f, 0.0f, 0.0f }, s2);
        EXPECT_TRUE(resp.mHit);
    }

    TEST(Sphere, responseNormal)
    {
        physics::Sphere s1{{0.0f, 0.0f, 0.0f}, 1.0f};
        physics::Sphere s2{{1.0f, 0.0f, 0.0f}, 1.0f};

        auto norms = physics::SphereVsSphereResponseNormals(s1, s2);

        ExpectVec3FltEq(norms.mShape1ResponseNormal, {-1.0f, 0.0f, 0.0f});
        ExpectVec3FltEq(norms.mShape2ResponseNormal, {1.0f, 0.0f, 0.0f});
    }

    TEST(Sphere, pointInSphere)
    {
        physics::Sphere s1{{0.0f, 0.0f, 0.0f}, 1.0f};
        glm::vec3 point = { 0.5f, 0.5f, 0.5f };

        EXPECT_TRUE(physics::PointInSphere(point, s1));
    }

    TEST(Sphere, pointOutsideSphere)
    {
        physics::Sphere s1{ {0.0f, 0.0f, 0.0f}, 1.0f };
        glm::vec3 point = { 1.1f, 0.5f, 0.5f };

        EXPECT_FALSE(physics::PointInSphere(point, s1));
    }

} // namespace physicsTests

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
