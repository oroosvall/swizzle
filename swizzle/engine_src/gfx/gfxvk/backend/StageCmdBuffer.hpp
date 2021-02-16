#ifndef RESOURCE_TRANSFER_HPP
#define RESOURCE_TRANSFER_HPP

#include "Vk.hpp"

#include <vector>

namespace swizzle::gfx
{

    struct StageMemoryBuffer
    {
        VkBuffer mBuffer;
        VkDeviceMemory mMemory;
        VkDeviceSize mAllocatedSize;
    };

    class StageCmdBuffer
    {
    public:
        StageCmdBuffer(const VkDevice& device, const VkCommandPool& cmdPool, const VkQueue& queue, const VkPhysicalDeviceMemoryProperties& deviceMemorProperties);
        virtual ~StageCmdBuffer();

        SwBool readyToSubmit() const;

        void beginStageRecording();
        void endAndSubmitRecording();

        StageMemoryBuffer& allocateStagingMemory(U8* data, VkDeviceSize memSize);

        void transitionImage(VkImage image);

        VkCommandBuffer getCmdBuffer() { return mCmdBuffer; }

    private:

        void clearStageBuffers();

        const VkDevice& mDevice;
        const VkCommandPool& mCmdPool;
        const VkQueue& mQueue;
        const VkPhysicalDeviceMemoryProperties& mDeviceMemorProperties;

        VkCommandBuffer mCmdBuffer;

        SwBool mRecordingStarted;

        std::vector<StageMemoryBuffer> mStageBuffers;

        VkFence mCompleteFence;

    };

}

#endif