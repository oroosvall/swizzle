#ifdef SW_LINUX_XCB

/* Include files */

#include <common/Common.hpp>
#include <swizzle/core/Window.hpp>

#include <iostream>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::core
{
    namespace platform
    {
        U32 getMonitorCount()
        {
            return 0u;
        }

        MonitorInfo getMonitorInfo(U32 index)
        {
            UNUSED_ARG(index);
            MonitorInfo res{};
            return res;
        }
    }
}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */

#endif