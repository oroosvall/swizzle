
/* Include files */

#include "QueryPool.hpp"
#include "Device.hpp"

#include <utils/BitUtil.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace vk
{
    StatisticsQuery::StatisticsQuery(common::Resource<Device> device)
        : mDevice(device)
        , mPipelineStatistics()
        , mQueryPool(VK_NULL_HANDLE)
        , mQueryCount(1u)
        , mStatisticsCount(7u)
        , mQueryBits(0u)
        , mEnabled(false)
        , mHasRequestedAfterEnable(false)
    {
        auto features = mDevice->getDeviceFeatures();
        VkQueryPoolCreateInfo createInfo = {};
        createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0u;
        createInfo.queryType = VkQueryType::VK_QUERY_TYPE_PIPELINE_STATISTICS;

        mQueryCount = 1u;
        mStatisticsCount = 7u;
        mQueryBits = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
                     VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT |
                     VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
                     VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
                     VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
                     VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT |
                     VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT;

        if (features.tessellationShader)
        {
            mQueryBits |= VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT |
                          VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT;
            mStatisticsCount += 2u;
        }

        createInfo.queryCount = mQueryCount;
        createInfo.pipelineStatistics = mQueryBits;

        mQueryPool = mDevice->createQueryPool(createInfo);
    }

    StatisticsQuery::~StatisticsQuery()
    {
        mDevice->destroyQueryPool(mQueryPool);
    }

    U32 StatisticsQuery::getQueryCount() const
    {
        return mQueryCount;
    }

    QueryData StatisticsQuery::getQuery() const
    {
        QueryData qd{};
        qd.mQuery = mQueryPool;
        qd.mQueryIndex = 0u;
        mHasRequestedAfterEnable = true;
        return qd;
    }

    void StatisticsQuery::resetQuery()
    {
        // Nothing to be done
    }

    void StatisticsQuery::enable(SwBool enable)
    {
        mEnabled = enable;
        mHasRequestedAfterEnable = false;
    }

    SwBool StatisticsQuery::isEnabled() const
    {
        return mEnabled;
    }

    SwBool StatisticsQuery::getQueryData(U64** outData, U64* inOutSize, SwBool awaitResults)
    {
        std::vector<U64> result(mStatisticsCount);
        VkQueryResultFlags flags = VkQueryResultFlagBits::VK_QUERY_RESULT_64_BIT;
        if (awaitResults && mHasRequestedAfterEnable)
        {
            flags |= VkQueryResultFlagBits::VK_QUERY_RESULT_WAIT_BIT;
        }
        VkResult res = vkGetQueryPoolResults(mDevice->getDeviceHandle(), mQueryPool, 0u, 1u, mStatisticsCount * sizeof(U64),
                              result.data(), sizeof(U64), flags);

        vk::LogVulkanError(res, "vkGetQueryPoolResults");

        mPipelineStatistics.mEnabledBits = mQueryBits;
        U64* addr = (U64*)&mPipelineStatistics.mInputAssemblyVertices;

        U32 resultOffset = 0u;
        for (U32 i = 0; i < 13u; ++i) // 13 is the number of bits set
        {
            if (utils::IsBitSet(mQueryBits, i))
            {
                addr[i] = result[resultOffset++];
            }
        }

        if (inOutSize && *inOutSize == sizeof(mPipelineStatistics))
        {
            if (outData)
            {
                *outData = (U64*)&mPipelineStatistics;
            }
        }
        return true;
    }

    // Timing query

    TimingQuery::TimingQuery(common::Resource<Device> device, U32 queryCount)
        : mDevice(device)
        , mPipelineStatistics()
        , mQueryPool(VK_NULL_HANDLE)
        , mQueryCount(queryCount)
        , mCurrentIndex(0u)
        , mEnabled(false)
        , mHasRequestedAfterEnable(false)
        , mTimestaps()
    {
        auto features = mDevice->getDeviceFeatures();
        VkQueryPoolCreateInfo createInfo = {};
        createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0u;
        createInfo.queryType = VkQueryType::VK_QUERY_TYPE_TIMESTAMP;
        createInfo.queryCount = mQueryCount;
        createInfo.pipelineStatistics = 0u;

        mQueryPool = mDevice->createQueryPool(createInfo);

        mTimestaps.resize(queryCount);
    }

    TimingQuery::~TimingQuery()
    {
        mDevice->destroyQueryPool(mQueryPool);
    }

    U32 TimingQuery::getQueryCount() const
    {
        return mQueryCount;
    }

    QueryData TimingQuery::getQuery() const
    {
        QueryData qd{};
        qd.mQuery = mQueryPool;
        qd.mQueryCount = mQueryCount;
        qd.mQueryIndex = mCurrentIndex;

        mHasRequestedAfterEnable = true;
        mCurrentIndex++;
        return qd;
    }

    void TimingQuery::resetQuery()
    {
        mCurrentIndex = 0u;
    }

    void TimingQuery::enable(SwBool enable)
    {
        mEnabled = enable;
        mHasRequestedAfterEnable = false;
    }
    SwBool TimingQuery::isEnabled() const
    {
        return mEnabled;
    }

    SwBool TimingQuery::getQueryData(U64** outData, U64* inOutSize, SwBool awaitResults)
    {
        VkQueryResultFlags flags = VkQueryResultFlagBits::VK_QUERY_RESULT_64_BIT;
        if (awaitResults && mHasRequestedAfterEnable)
        {
            flags |= VkQueryResultFlagBits::VK_QUERY_RESULT_WAIT_BIT;
        }
        if (mEnabled && mCurrentIndex != 0u)
        {
            VkResult res = vkGetQueryPoolResults(mDevice->getDeviceHandle(), mQueryPool, 0u, mCurrentIndex,
                                                 mCurrentIndex * sizeof(U64), mTimestaps.data(), sizeof(U64), flags);

            vk::LogVulkanError(res, "vkGetQueryPoolResults");

            if (inOutSize && outData)
            {
                *inOutSize = mCurrentIndex;
                *outData = mTimestaps.data();
            }

            return true;
        }
        return false;
    }

} // namespace vk

/* Class Protected Function Definition */

/* Class Private Function Definition */
