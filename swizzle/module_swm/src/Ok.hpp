#ifndef OK_HPP
#define OK_HPP

/* Include files */

#include <swm/Swm.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace swm
{
    inline SwBool Ok(Result result)
    {
        return (result == Result::Success);
    }
}

#endif
