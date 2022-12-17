
#include "Stats.hpp"

#include <optick/optick.h>
#include <utils/FpsCounter.hpp>
#include <utils/StringUtils.hpp>

std::string GetStatisticsText(common::Resource<swizzle::gfx::GfxContext> context,
                              common::Resource<swizzle::gfx::CommandBuffer> cmdBuff,
                              common::Resource<swizzle::gfx::Swapchain> swapchain, utils::FpsCounter& fpsCounter)
{
    std::string statisticsString;
    statisticsString.reserve(1024);

    statisticsString = "Frames: " + std::to_string(swapchain->getFrameCounter()) + "\n";
    statisticsString += "FPS: " + std::to_string(fpsCounter.getFps()) + "\n\n";

    statisticsString += std::string(context->getSelectedDeviceName()) + "\n";

    auto iter = context->getStatisticsIterator();

    do
    {
        OPTICK_EVENT("Parse Stats");
        if (iter->getType() == swizzle::gfx::GfxStatsType::MemoryStats)
        {
            swizzle::gfx::MemoryStatistics* memStat = (swizzle::gfx::MemoryStatistics*)iter->getStatisticsData();

            statisticsString += "Memory Heap: " + std::string(memStat->mName) + "\n";
            statisticsString +=
                "  Mem: " + utils::toMemoryString(memStat->mUsed) + "/" + utils::toMemoryString(memStat->mSize);
            statisticsString += "; Allocs: " + std::to_string(memStat->mNumAllocations) + "p, " +
                                std::to_string(memStat->mNumVirtualAllocations) + "v\n";
        }
        else if (iter->getType() == swizzle::gfx::GfxStatsType::DeviceStats)
        {
            swizzle::gfx::DeviceStatistics* devStats = (swizzle::gfx::DeviceStatistics*)iter->getStatisticsData();
            statisticsString += "Device\n";
            statisticsString += "  Num Staged Objects: " + std::to_string(devStats->mNumStagedObjects) + "\n";
            statisticsString += "  Num Textures: " + std::to_string(devStats->mNumTextures) + "\n";
            statisticsString += "  Num Buffers: " + std::to_string(devStats->mNumBuffers) + "\n";
            statisticsString += "  Pipelines: " + std::to_string(devStats->mNumPipelines) + "\n";
        }
        else if (iter->getType() == swizzle::gfx::GfxStatsType::InstanceStats)
        {
            swizzle::gfx::InstanceStatistics* instStats = (swizzle::gfx::InstanceStatistics*)iter->getStatisticsData();
            statisticsString += "Instance\n";
            statisticsString += "  Alloc Count " + std::to_string(instStats->mAllocCount) + "\n";
            statisticsString += "  Internal Alloc Count " + std::to_string(instStats->mInternalAllocCount) + "\n";
        }
        else if (iter->getType() == swizzle::gfx::GfxStatsType::GfxPipelineStats)
        {
            swizzle::gfx::GfxPipelineStatistics* gfxStats =
                (swizzle::gfx::GfxPipelineStatistics*)iter->getStatisticsData();
            statisticsString += "Graphics Pipeline statistics\n";
            statisticsString += "  Input Assemby Vertices " + std::to_string(gfxStats->mInputAssemblyVertices) + "\n";
            statisticsString +=
                "  Input Assemby Primitives " + std::to_string(gfxStats->mInputAssemblyPrimitives) + "\n";
            statisticsString +=
                "  Vertex Shader Invocations " + std::to_string(gfxStats->mVertexShaderInvocations) + "\n";
            statisticsString += "  Clipping Invocations " + std::to_string(gfxStats->mClippingInvocations) + "\n";
            statisticsString += "  Clipping Primitives " + std::to_string(gfxStats->mClippingInvocations) + "\n";
            statisticsString +=
                "  Fragment Shader Invocations " + std::to_string(gfxStats->mFragmentShaderInvocations) + "\n";
            statisticsString += "  Tesselation Control Shader Patches " +
                                std::to_string(gfxStats->mTesselationControlShaderPatches) + "\n";
            statisticsString += "  Tesselation Evaluation Shader Invocations " +
                                std::to_string(gfxStats->mTesselationEvaluationShaderInvocations) + "\n";
            statisticsString +=
                "  Compute Shader Invocations " + std::to_string(gfxStats->mComputeShaderInvocations) + "\n";
        }

    } while (iter->next());

    statisticsString += "Draw call count: " + std::to_string(cmdBuff->getDrawCount()) + "\n";
    statisticsString += "Vertex count: " + std::to_string(cmdBuff->getVertCount()) + "\n";
    statisticsString += "Triangle count: " + std::to_string(cmdBuff->getTriCount()) + "\n";

    return statisticsString;
}