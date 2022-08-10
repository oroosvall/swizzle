#ifndef VK_CONTEXT_HPP
#define VK_CONTEXT_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/Context.hpp>

#include "res/Instance.hpp"
#include "res/Device.hpp"
#include "res/CmdPool.hpp"
#include "res/Fence.hpp"

#include <vector>
#include <string>

#include <utils/FrameAllocator.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::gfx
{
    class VkGfxContext : public GfxContext
    {
    public:

        VkGfxContext(const GfxContextCreateInfo& createInfo);
        virtual ~VkGfxContext();

        virtual U32 getDeviceCount() override;
        virtual const SwChar* getDeviceName(U32 deviceIndex) override;
        virtual const SwChar* getSelectedDeviceName() override;
        virtual SwBool initializeDevice(const GfxContextInitializeInfo& createInfo) override;

        virtual void waitIdle() override;

        virtual GfxStatistics getStatistics() override;
        virtual common::Resource<swizzle::core::StatisticsIterator<swizzle::gfx::GfxStatsType>> getStatisticsIterator() override;

        virtual common::Resource<Buffer> createBuffer(BufferType type) override;
        virtual common::Resource<CommandBuffer> createCommandBuffer(U32 swapCount) override;
        virtual common::Resource<Swapchain> createSwapchain(common::Resource<core::SwWindow> window, U32 swapCount) override;
        virtual common::Resource<Texture> createTexture(U32 width, U32 height, U32 channels, const U8* data = nullptr) override;
        virtual common::Resource<Texture> createCubeMapTexture(U32 width, U32 height, U32 channels, const U8* data = nullptr) override;

        virtual common::Resource<FrameBuffer> createFramebuffer(const FrameBufferCreateInfo& fboInfo) override;

        virtual void submit(common::Resource<CommandBuffer>* cmdBuffer, U32 cmdBufferCount,
                    common::Resource<Swapchain> swapchain) override;

        virtual common::Resource<Shader> createShader(common::Resource<FrameBuffer> framebuffer, const ShaderAttributeList& attribs) override;
        virtual common::Resource<Shader> createShader(common::Resource<Swapchain> swapchain, const ShaderAttributeList& attribs) override;

        virtual common::Resource<Material> createMaterial(common::Resource<Shader> shader) override;

        void resetCommandPool();

        void updateHeapBudget();

    private:

        void addExtensionIfExisting(std::vector<const char*>& extensions,
                                    const std::vector<std::string>& availableExtensions, const char* extensionName);

        common::Resource<vk::Fence> getFence();

        common::Resource<vk::Instance> mVkInstance;
        common::Resource<vk::Device> mVkDevice;
        common::Resource<vk::CmdPool> mCmdPool;

        VkQueue mQueue;

        utils::FrameAllocator mFrameAllocator;
        std::vector<common::Resource<vk::Fence>> mFences;

    };

}

/* Function Declaration */

#endif