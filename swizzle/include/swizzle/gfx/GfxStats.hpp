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
        InstanceStats,
        GfxPipelineStats,
        GfxTiming
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
        U32 mNumDescriptors;
    };

    struct GfxPipelineStatistics : public swizzle::core::StatsBase
    {
        U64 mEnabledBits; // bit field mapping enabled fields fron the statistics
        U64 mInputAssemblyVertices; // bit 0
        U64 mInputAssemblyPrimitives; // bit 1
        U64 mVertexShaderInvocations; // bit 2
        U64 mGeometryShaderInvokations; // bit 3
        U64 mGeometryShaderPrimitives; // bit 4
        U64 mClippingInvocations; // bit 5
        U64 mClippingPrimitives; // bit 6
        U64 mFragmentShaderInvocations; // bit 7
        U64 mTesselationControlShaderPatches; // bit 8
        U64 mTesselationEvaluationShaderInvocations; // bit 9
        U64 mComputeShaderInvocations; // bit 10
        U64 mTaskShaderInvocations; // bit 11
        U64 mMeshShaderInvocations; // bit 12
    };

    struct GfxTimingStatistics : public swizzle::core::StatsBase
    {
        U64 mSampleCount;
        U64 mNsPerSample;
        U64* mSamples;
    };

    // TODO: remove me
    struct GfxStatistics
    {
        U64 mGpuMemoryUsage;
        U64 mCpuMemoryUsage;
        U32 mStagedObjects;
        U32 mNumTextures;
        U32 mNumBuffers;
    };
}

/* Class Declaration */

/* Function Declaration */

#endif
