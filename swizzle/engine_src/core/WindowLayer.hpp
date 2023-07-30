
#pragma once

/* Include files */

#include <common/Common.hpp>
#include <swizzle/core/Window.hpp>

/* Defines */

/* Typedefs/enums */

namespace core = swizzle::core;

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace plf::window
{
    SwBool init();
    void cleanup();

    void pollWindowEvents();
}