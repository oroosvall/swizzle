#ifndef STATS_HPP
#define STATS_HPP

/* Include files */

#include "res/Device.hpp"

#include <swizzle/gfx/GfxStats.hpp>

#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::gfx
{

    class StatsIterator : public swizzle::core::StatisticsIterator<GfxStatsType>
    {
    public:
        StatsIterator(common::Resource<vk::Device> device);

        virtual ~StatsIterator();

        virtual SwBool next() override;

        virtual GfxStatsType getType() override;
        virtual swizzle::core::StatsBase* getStatisticsData() override;

        void addMemoryStats();
        void addDeviceStats();
        void addInstanceStats();
        void addPipelineStats();

    private:

        common::Resource<vk::Device> mDevice;

        U64 mIndex;
        std::vector<std::pair<GfxStatsType, swizzle::core::StatsBase*>> mStats;

    };

}

/* Function Declaration */

#endif
