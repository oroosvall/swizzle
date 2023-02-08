
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
    TEST(Oobb, overlaps)
    {
        physics::OOBB s1{ {0.0f, 0.0f, 0.0f},{1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };
        physics::OOBB s2{ {1.0f, 1.0f, 1.0f},{1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };

        auto collides = physics::OobbVsOobb(s1, s2);
        EXPECT_TRUE(collides);
    }

    TEST(Oobb, noOverlap)
    {
        physics::OOBB s1{ {0.0f, 0.0f, 0.0f},{1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };
        physics::OOBB s2{ {3.0f, 3.0f, 3.0f},{1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };

        auto collides = physics::OobbVsOobb(s1, s2);
        EXPECT_FALSE(collides);
    }

} // namespace physicsTests

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
