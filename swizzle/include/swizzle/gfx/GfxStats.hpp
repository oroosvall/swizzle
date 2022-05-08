#ifndef GFX_STATS_HPP
#define GFX_STATS_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/core/Statistics.hpp>

/* Defines */

/* Typedefs/enums */

namespace swizzle::gfx
{
    enum class GfxStatsType
    {
        InvalidStats,
        MemoryStats,
        DeviceStats,
        InstanceStats
    };
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::gfx
{

    struct MemoryStatistics : public swizzle::core::StatsBase
    {
        const SwChar* mName;
        U64 mUsed;
        U64 mFree;
        U64 mSize;
        U32 mNumAllocations;
        U32 mNumVirtualAllocations;
    };

    struct InstanceStatistics : public swizzle::core::StatsBase
    {
        U64 mAllocCount;
        U64 mInternalAllocCount;
    };

    struct DeviceStatistics : public swizzle::core::StatsBase
    {
        U32 mNumPipelines;
        U32 mNumBuffers;
        U32 mNumTextures;
        U32 mNumStagedObjects;
    };
}

/* Class Declaration */

/* Function Declaration */

#endif
