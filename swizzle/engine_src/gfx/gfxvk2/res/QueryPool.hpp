#ifndef QUERY_POOL_HPP
#define QUERY_POOL_HPP

/* Include files */

#include <swizzle/gfx/GfxStats.hpp>

#include "../backend/Vk.hpp"
#include "_fwDecl.hpp"

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
} // namespace vk

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
    class Query
    {
    public:
        virtual ~Query(){};

        /// <summary>
        /// Returns the number of queries in the active pool.
        /// </summary>
        /// <returns></returns>
        virtual U32 getQueryCount() const = 0;

        /// <summary>
        /// Returns the active query vulkan handle.
        /// </summary>
        /// <returns></returns>
        virtual QueryData getQuery() const = 0;

        virtual void resetQuery() = 0;

        virtual void enable(SwBool enable) = 0;
        virtual SwBool isEnabled() const = 0;

        /// <summary>
        /// Retrievs pipeline statistics.
        ///
        /// Thread Safe: No, this function shall only be called by one thread.
        /// </summary>
        /// <param name="awaitResult">This parameter forces the function to wait for results</param>
        /// <param name="awaitResult">This parameter forces the function to wait for results</param>
        /// <param name="awaitResult">This parameter forces the function to wait for results</param>
        /// <returns>true if data was successfully returned</returns>
        virtual SwBool getQueryData(U64** outData, U64* inOutSize, SwBool awaitResults) = 0;
    };

    class StatisticsQuery : public Query
    {
    public:
        StatisticsQuery(common::Resource<Device> device);
        virtual ~StatisticsQuery();

        virtual U32 getQueryCount() const override;
        virtual QueryData getQuery() const override;

        virtual void resetQuery() override;

        virtual void enable(SwBool enable) override;
        virtual SwBool isEnabled() const override;

        virtual SwBool getQueryData(U64** outData, U64* inOutSize, SwBool awaitResults) override;

    private:
        common::Resource<Device> mDevice;
        swizzle::gfx::GfxPipelineStatistics mPipelineStatistics;

        VkQueryPool mQueryPool;
        U32 mQueryCount;
        U32 mStatisticsCount;
        U32 mQueryBits;

        SwBool mEnabled;

        mutable std::atomic_bool mHasRequestedAfterEnable;
    };

    class TimingQuery : public Query
    {
    public:
        TimingQuery(common::Resource<Device> device, U32 queryCount);
        virtual ~TimingQuery();

        virtual U32 getQueryCount() const override;
        virtual QueryData getQuery() const override;

        virtual void resetQuery() override;

        virtual void enable(SwBool enable) override;
        virtual SwBool isEnabled() const override;

        virtual SwBool getQueryData(U64** outData, U64* inOutSize, SwBool awaitResults) override;

    private:
        common::Resource<Device> mDevice;
        swizzle::gfx::GfxPipelineStatistics mPipelineStatistics;

        VkQueryPool mQueryPool;
        U32 mQueryCount;
        mutable U32 mCurrentIndex;

        SwBool mEnabled;

        mutable std::atomic_bool mHasRequestedAfterEnable;
        std::vector<U64> mTimestaps;
    };

} // namespace vk

/* Function Declaration */

#endif
