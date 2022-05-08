
/* Include files */

#include "Stats.hpp"

#include "backend/VkDebug.hpp"

#include <optick/optick.h>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace swizzle::gfx
{

    StatsIterator::StatsIterator(common::Resource<vk::Device> device)
        : mDevice(device)
        , mIndex(0u)
        , mStats()
    {
    }

    StatsIterator::~StatsIterator()
    {
        mStats.clear();
    }

    SwBool StatsIterator::next()
    {
        mIndex++;
        return mIndex < mStats.size();
    }

    GfxStatsType StatsIterator::getType()
    {
        GfxStatsType ret = GfxStatsType::InvalidStats;
        if (mIndex < mStats.size())
        {
            ret = mStats[mIndex].first;
        }
        return ret;
    }

    swizzle::core::StatsBase* StatsIterator::getStatisticsData()
    {
        swizzle::core::StatsBase* ret = nullptr;
        if (mIndex < mStats.size())
        {
            ret = mStats[mIndex].second;
        }
        return ret;
    }

    void StatsIterator::addMemoryStats()
    {
        OPTICK_EVENT("StatsIterator::addMemoryStats()");
        U32 count = mDevice->getMemoryHeapCount();

        for (U32 i = 0u; i < count; ++i)
        {
            mStats.push_back(std::make_pair(GfxStatsType::MemoryStats, mDevice->getHeapStatsistics(i)));
        }
    }

    void StatsIterator::addDeviceStats()
    {
        OPTICK_EVENT("StatsIterator::addDeviceStats()");
        mStats.push_back(std::make_pair(GfxStatsType::DeviceStats, mDevice->getDeviceStats()));
    }

    void StatsIterator::addInstanceStats()
    {
        OPTICK_EVENT("StatsIterator::addInstanceStats()");
        static InstanceStatistics stats = {};
        stats.mAllocCount = vk::getAllocCount();
        stats.mInternalAllocCount = vk::getAllocInternalCount();
        mStats.push_back(std::make_pair(gfx::GfxStatsType::InstanceStats, &stats));

    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
