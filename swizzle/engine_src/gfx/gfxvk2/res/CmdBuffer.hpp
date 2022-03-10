#ifndef CMD_BUFFER_HPP
#define CMD_BUFFER_HPP

/* Include files */

#include <swizzle/gfx/CommandBuffer.hpp>

#include "_fwDecl.hpp"
#include "../backend/Vk.hpp"

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

        virtual void begin() override;
        virtual void end() override;

        virtual void uploadTexture(common::Resource<swizzle::gfx::Texture> texture) override;

        virtual void beginRenderPass(common::Resource<swizzle::gfx::Swapchain> swp) override;
        virtual void beginRenderPass(common::Resource<swizzle::gfx::FrameBuffer> fbo) override;
        virtual void endRenderPass() override;

        virtual void bindShader(common::Resource<swizzle::gfx::Shader> shader) override;
        virtual void bindMaterial(common::Resource<swizzle::gfx::Shader> shader,
                                  common::Resource<swizzle::gfx::Material> material) override;
        virtual void setShaderConstant(common::Resource<swizzle::gfx::Shader> shader, U8* data, U32 size) override;
        virtual void setViewport(U32 x, U32 y) override;

        virtual void bindVertexBuffer(common::Resource<swizzle::gfx::Buffer> buffer) override;
        virtual void bindIndexBuffer(common::Resource<swizzle::gfx::Buffer> buffer, SwBool bitSize16) override;

        virtual void draw(common::Resource<swizzle::gfx::Buffer> buffer) override;
        virtual void drawIndexed(common::Resource<swizzle::gfx::Buffer> buffer,
                                 common::Resource<swizzle::gfx::Buffer> index) override;

        virtual void drawNoBind(U32 vertexCount, U32 first) override;
        virtual void drawIndexedNoBind(U32 vertexCount, U32 first, U32 vertOffset) override;

        virtual const swizzle::gfx::ScissorInfo pushScissorRegion(S32 x, S32 y, S32 w, S32 h) override;
        virtual void popScisorInfo(const swizzle::gfx::ScissorInfo& sci) override;

        virtual void setScissor(S32 x, S32 y, S32 w, S32 h) override;

    private:

        std::shared_ptr<CmdBuffer> shared_from_this()
        {
            return std::dynamic_pointer_cast<CmdBuffer>(swizzle::gfx::CommandBuffer::shared_from_this());
        }

        VkCommandBuffer getCmdBuffer(U32 index);
        common::Resource<vk::Fence> getFence(U32 index);
        //VkFence getFence(U32 index);
        void incrementActiveIndex();

        common::Resource<Device> mDevice;
        common::Resource<CmdPool> mCmdPool;

        U32 mBufferCount;
        U32 mActiveIndex;
        U64 mFrameCounter;
        VkCommandBuffer* mCommandBuffers;
        common::Resource<vk::Fence>* mFences;
        //VkFence* mFences;

        U32 mDrawCount;

        swizzle::gfx::ScissorInfo mScissorInfo;
    };
}

/* Function Declaration */

#endif
