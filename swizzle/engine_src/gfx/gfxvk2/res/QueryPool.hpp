#ifndef QUERY_POOL_HPP
#define QUERY_POOL_HPP

/* Include files */

#include <swizzle/gfx/GfxStats.hpp>

#include "_fwDecl.hpp"
#include "../backend/Vk.hpp"

#include <atomic>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace vk
{
    struct QueryData
    {
        VkQueryPool mQuery;
        U32 mQueryIndex;
        U32 mQueryCount;
    };
}

/* Class Declaration */

namespace vk
{
    /// <summary>
    /// Query pool to manage pipeline statistics.
    /// This class is constructed by the Device.
    /// This class is used by the CmdBuffer.
    ///
    /// Thread safe: Specific functions
    /// </summary>
    class QueryPool
    {
    public:
        QueryPool(common::Resource<Device> device, U32 queryCount);
        ~QueryPool();

        /// <summary>
        /// Returns the number of queries in the active pool.
        /// </summary>
        /// <returns></returns>
        U32 getQueryCount() const;

        /// <summary>
        /// Returns the active query vulkan handle.
        /// </summary>
        /// <returns></returns>
        VkQueryPool getHandle() const;

        /// <summary>
        /// Retrievs pipeline statistics.
        ///
        /// Thread Safe: No, this function shall only be called by one thread.
        /// </summary>
        /// <param name="awaitResult">This parameter forces the function to wait for results</param>
        /// <returns>Pointer to internal structure containing the pipeline statistics information</returns>
        swizzle::gfx::GfxPipelineStatistics* getGfxPipelineStatistics(SwBool awaitResult);

        void enableStatistics(SwBool enable);
        SwBool isStatisticsEnabled() const;

    private:
        common::Resource<Device> mDevice;
        swizzle::gfx::GfxPipelineStatistics mPipelineStatistics;

        VkQueryPool mQueryPool;
        U32 mQueryCount;
        U32 mStatisticsCount;
        U32 mQueryBits;

        SwBool mStatisticsEnabled;

        mutable std::atomic_bool mHasRequestedAfterEnable;
    };
}

/* Function Declaration */

#endif
