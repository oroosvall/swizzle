
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

    QueryPool::QueryPool(common::Resource<Device> device, U32 queryCount)
        : mDevice(device)
        , mPipelineStatistics()
        , mQueryPool(VK_NULL_HANDLE)
        , mQueryCount(queryCount)
        , mStatisticsCount(0u)
        , mQueryBits(0u)
        , mStatisticsEnabled(false)
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
        mQueryBits =
            VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
            VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT |
            VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
            VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
            VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
            VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT |
            VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT;

        if (features.tessellationShader)
        {
            mQueryBits |=
                VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT |
                VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT;
            mStatisticsCount += 2u;
        }

        createInfo.queryCount = mQueryCount;
        createInfo.pipelineStatistics = mQueryBits;

        mQueryPool = mDevice->createQueryPool(createInfo);
        //vkResetQueryPool(mDevice->getDeviceHandle(), mQueryPool, 0u, mQueryCount); // Vulkan 1.2 feature
    }

    QueryPool::~QueryPool()
    {
        mDevice->destroyQueryPool(mQueryPool);
    }

    U32 QueryPool::getQueryCount() const
    {
        return mQueryCount;
    }

    VkQueryPool QueryPool::getHandle() const
    {
        mHasRequestedAfterEnable = true;
        return mQueryPool;
    }

    swizzle::gfx::GfxPipelineStatistics* QueryPool::getGfxPipelineStatistics(SwBool awaiResult)
    {
        std::vector<U64> result(mStatisticsCount);
        VkQueryResultFlags flags = VkQueryResultFlagBits::VK_QUERY_RESULT_64_BIT;
        if (awaiResult && mHasRequestedAfterEnable)
        {
            flags |= VkQueryResultFlagBits::VK_QUERY_RESULT_WAIT_BIT;
        }
        vkGetQueryPoolResults(mDevice->getDeviceHandle(), mQueryPool, 0u, 1u, mStatisticsCount * sizeof(U64), result.data(), sizeof(U64), flags);

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

        return &mPipelineStatistics;
    }

    void QueryPool::enableStatistics(SwBool enable)
    {
        mStatisticsEnabled = enable;
        mHasRequestedAfterEnable = false;
    }

    SwBool QueryPool::isStatisticsEnabled() const
    {
        return mStatisticsEnabled;
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */

