
#pragma once

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/GfxDevice.hpp>

#include "res/CmdPool.hpp"
#include "res/Device.hpp"
#include "res/Fence.hpp"
#include "res/Instance.hpp"

#include <string>
#include <vector>

#include <utils/FrameAllocator.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::gfx
{
    class VkGfxDevice : public GfxDevice
    {
    public:
        VkGfxDevice(common::Resource<vk::Instance> instance,
                    common::Resource<vk::Device> device);
        virtual ~VkGfxDevice();

        virtual SwBool hasMeshShaderSupport() override;

        virtual void waitIdle() override;

        virtual GfxStatistics getStatistics() override;
        virtual common::Resource<swizzle::core::StatisticsIterator<swizzle::gfx::GfxStatsType>>
            getStatisticsIterator() override;

        virtual common::Resource<Buffer> createBuffer(BufferType type) override;
        virtual common::Resource<CommandBuffer> createCommandBuffer(U32 swapCount) override;
        virtual common::Resource<Swapchain> createSwapchain(common::Resource<core::SwWindow> window,
                                                            U32 swapCount) override;
        virtual common::Resource<Texture> createTexture(U32 width, U32 height, U32 channels, SwBool f32,
                                                        const U8* data = nullptr) override;
        virtual common::Resource<Texture> createCubeMapTexture(U32 width, U32 height, U32 channels,
                                                               const U8* data = nullptr) override;

        virtual common::Resource<FrameBuffer> createFramebuffer(const FrameBufferCreateInfo& fboInfo) override;

        virtual void submit(common::Resource<CommandBuffer>* cmdBuffer, U32 cmdBufferCount,
                            common::Resource<Swapchain> swapchain) override;

        virtual common::Resource<Shader> createShader(common::Resource<FrameBuffer> framebuffer,
                                                      const swizzle::gfx::ShaderType type,
                                                      const ShaderAttributeList& attribs) override;
        virtual common::Resource<Shader> createShader(common::Resource<Swapchain> swapchain,
                                                      const swizzle::gfx::ShaderType type,
                                                      const ShaderAttributeList& attribs) override;

        virtual common::Resource<Material> createMaterial(common::Resource<Shader> shader,
                                                          SamplerMode samplerMode) override;

        virtual void enablePipelineStatistics(SwBool enable) override;
        virtual void enableGpuTiming(SwBool enable) override;

        void resetCommandPool();

        void updateHeapBudget();

    private:
        SwBool addExtensionIfExisting(std::vector<const char*>& extensions,
                                      const std::vector<std::string>& availableExtensions, const char* extensionName);

        common::Resource<vk::Fence> getFence();

        common::Resource<vk::Instance> mVkInstance;
        common::Resource<vk::Device> mVkDevice;
        common::Resource<vk::CmdPool> mCmdPool;

        VkQueue mQueue;

        utils::FrameAllocator mFrameAllocator;
        std::vector<common::Resource<vk::Fence>> mFences;

        SwBool mMeshShaderSupported;
    };

} // namespace swizzle::gfx

/* Function Declaration */
