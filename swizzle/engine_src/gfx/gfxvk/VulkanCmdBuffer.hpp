#ifndef VULKAN_CMD_BUFFER_HPP
#define VULKAN_CMD_BUFFER_HPP

/* Include files */

#include "backend/Vk.hpp"
#include "backend/VkContainer.hpp"

#include <swizzle/gfx/CommandBuffer.hpp>
#include <swizzle/gfx/Swapchain.hpp>
#include <swizzle/gfx/Material.hpp>

#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::gfx
{
    struct CmdBuffer
    {
        VkCommandBuffer mCmdBuffer;
        VkFence mFence;

        std::vector<std::shared_ptr<VkBufferInt>> mBufferReferences;
    };
}

/* Class Declaration */

namespace swizzle::gfx
{

    class VulkanCommandBuffer : public CommandBuffer
    {
    public:
        VulkanCommandBuffer(const VkContainer vkObjects, U32 swapCount);
        virtual ~VulkanCommandBuffer();

        virtual U32 getDrawCount() override;
        virtual U64 getVertCount() override;
        virtual U64 getTriCount() override;

        virtual void reset(bool hardReset) override;

        virtual void begin() override;
        virtual void end() override;

        virtual void uploadTexture(common::Resource<Texture> texture) override;

        virtual void submit(common::Resource<Swapchain> swp = nullptr) override;

        virtual void beginRenderPass(common::Resource<Swapchain> swp) override;
        virtual void beginRenderPass(common::Resource<FrameBuffer> fbo) override;
        virtual void endRenderPass() override;

        virtual void bindShader(common::Resource<Shader> shader) override;
        virtual void bindMaterial(common::Resource<Shader> shader, common::Resource<Material> material) override;
        virtual void setShaderConstant(common::Resource<Shader> shader, U8* data, U32 size) override;
        virtual void setViewport(U32 x, U32 y) override;

        virtual void draw(common::Resource<Buffer> buffer) override;
        virtual void drawIndexed(common::Resource<Buffer> buffer, common::Resource<Buffer> index) override;

    private:

        CmdBuffer& getCmdBuffer(U32 index);

        void createVkResources();

        const VkContainer mVkObjects;

        const U32 mSwapCount;

        std::vector<CmdBuffer> mCmdBuffers;

        U32 mActiveIndex;

        U32 mDrawCount;
        U64 mVertCount;
        U64 mTriCount;
    };

}

/* Function Declaration */

#endif