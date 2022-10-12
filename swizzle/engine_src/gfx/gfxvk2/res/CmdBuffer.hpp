#ifndef CMD_BUFFER_HPP
#define CMD_BUFFER_HPP

/* Include files */

#include <swizzle/gfx/CommandBuffer.hpp>

#include "../backend/Vk.hpp"
#include "_fwDecl.hpp"

#include <memory>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{
    class CmdBuffer : public swizzle::gfx::CommandBuffer
    {
    public:
        CmdBuffer(common::Resource<vk::Device> device, common::Resource<vk::CmdPool> cmdPool, U32 bufferCount);
        virtual ~CmdBuffer();

        U64 getFrameCount();

        SwBool isFrameInProgress(U64 frameCounter);

        VkCommandBuffer getActiveCommandBuffer();
        void setWaitFence(common::Resource<vk::Fence> fence);

        virtual U32 getDrawCount() override;
        virtual U64 getVertCount() override;
        virtual U64 getTriCount() override;

        virtual void reset(bool hardReset) override;

        virtual common::Unique<swizzle::gfx::CommandTransaction> begin() override;
        virtual void end(common::Unique<swizzle::gfx::CommandTransaction>&& transaction) override;

        virtual common::Unique<swizzle::gfx::DrawCommandTransaction>
            beginRenderPass(common::Resource<swizzle::gfx::Swapchain> swp,
                            common::Unique<swizzle::gfx::CommandTransaction>&& transaction) override;
        virtual common::Unique<swizzle::gfx::DrawCommandTransaction>
            beginRenderPass(common::Resource<swizzle::gfx::FrameBuffer> fbo,
                            common::Unique<swizzle::gfx::CommandTransaction>&& transaction) override;
        virtual common::Unique<swizzle::gfx::CommandTransaction>
            endRenderPass(common::Unique<swizzle::gfx::DrawCommandTransaction>&& transaction) override;

    private:
        std::shared_ptr<CmdBuffer> shared_from_this()
        {
            return std::dynamic_pointer_cast<CmdBuffer>(swizzle::gfx::CommandBuffer::shared_from_this());
        }

        VkCommandBuffer getCmdBuffer(U32 index);
        common::Resource<vk::Fence> getFence(U32 index);
        // VkFence getFence(U32 index);
        void incrementActiveIndex();

        common::Resource<Device> mDevice;
        common::Resource<CmdPool> mCmdPool;

        U32 mBufferCount;
        U32 mActiveIndex;
        U64 mFrameCounter;
        VkCommandBuffer* mCommandBuffers;
        common::Resource<vk::Fence>* mFences;
        common::Resource<LifetimeToken> mLifetimeToken;
        // VkFence* mFences;

        U32 mDrawCount;

        SwBool mCachedGatherPipelineStatistics;
    };
} // namespace vk

/* Function Declaration */

#endif
