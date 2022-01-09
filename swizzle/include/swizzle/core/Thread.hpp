#ifndef THREAD_HPP
#define THREAD_HPP

/* Include files */

#include <common/Common.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::core
{
    class IRunnable
    {
    public:
        virtual void run() = 0;
    };

    class Thread
    {
    public:
        void start();
        void stop();
    };
}

/* Function Declaration */


#endif