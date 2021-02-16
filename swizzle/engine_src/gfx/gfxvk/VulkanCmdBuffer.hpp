#ifndef VULKAN_CMD_BUFFER_HPP
#define VULKAN_CMD_BUFFER_HPP

#include "backend/Vk.hpp"
#include "backend/VulkanObjectContainer.hpp"

#include <swizzle/gfx/CommandBuffer.hpp>
#include <swizzle/gfx/Swapchain.hpp>
#include <swizzle/gfx/Material.hpp>

namespace swizzle::gfx
{

    class VulkanCommandBuffer : public CommandBuffer
    {
    public:
        VulkanCommandBuffer(const VulkanObjectContainer& vkObjects);
        virtual ~VulkanCommandBuffer();

        virtual void reset(bool hardReset) override;

        virtual void begin() override;
        virtual void end() override;

        virtual void submit(core::Resource<Swapchain> swp = nullptr) override;

        virtual void beginRenderPass(core::Resource<Swapchain> swp) override;
        virtual void beginRenderPass(core::Resource<FrameBuffer> fbo) override;
        virtual void endRenderPass() override;

        virtual void bindShader(core::Resource<Shader> shader) override;
        virtual void bindMaterial(core::Resource<Shader> shader, core::Resource<Buffer> material) override;
        virtual void setShaderConstant(core::Resource<Shader> shader, U8* data, U32 size) override;
        virtual void setViewport(U32 x, U32 y) override;

        virtual void draw(core::Resource<Buffer> buffer) override;

        const VulkanObjectContainer& mVkObjects;


        VkCommandBuffer mCmdBuffer;
        VkFence mCompleteFence;

    };

}

#endif