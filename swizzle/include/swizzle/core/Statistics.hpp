#ifndef STATISTICS_HPP
#define STATISTICS_HPP

/* Include files */

#include <common/Common.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::core
{
    struct StatsBase
    {
    };
} // namespace swizzle::core

/* Class Declaration */

namespace swizzle::core
{
    // TODO: move to core
    template <typename T> class StatisticsIterator
    {
    public:
        virtual ~StatisticsIterator() {}

        virtual SwBool next() = 0;

        virtual T getType() = 0;
        virtual StatsBase* getStatisticsData() = 0;
    };
} // namespace swizzle::core

/* Function Declaration */

#endif
