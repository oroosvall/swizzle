
#ifndef GFX_DEVICE_HPP
#define GFX_DEVICE_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/Api.hpp>
#include <swizzle/core/Window.hpp>

#include <swizzle/gfx/GfxBuffer.hpp>
#include <swizzle/gfx/CommandBuffer.hpp>
#include <swizzle/gfx/FrameBuffer.hpp>
#include <swizzle/gfx/Swapchain.hpp>
#include <swizzle/gfx/Texture.hpp>

#include <swizzle/gfx/GfxStats.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::gfx
{
    class GfxDevice
    {
    public:
        virtual SwBool hasMeshShaderSupport() = 0;

        virtual void waitIdle() = 0;

        virtual GfxStatistics getStatistics() = 0;
        virtual common::Resource<swizzle::core::StatisticsIterator<gfx::GfxStatsType>> getStatisticsIterator() = 0;

        virtual common::Resource<GfxBuffer> createBuffer(GfxBufferType type) = 0;
        virtual common::Resource<CommandBuffer> createCommandBuffer(U32 swapCount) = 0;
        virtual common::Resource<Swapchain> createSwapchain(common::Resource<core::SwWindow> window, U32 swapCount) = 0;
        virtual common::Resource<Texture> createTexture(U32 width, U32 height, U32 channels, SwBool f32,
                                                        const U8* data = nullptr) = 0;
        virtual common::Resource<Texture> createCubeMapTexture(U32 width, U32 height, U32 channels,
                                                               const U8* data = nullptr) = 0;

        virtual common::Resource<FrameBuffer> createFramebuffer(const FrameBufferCreateInfo& fboInfo) = 0;

        virtual void submit(common::Resource<CommandBuffer>* cmdBuffer, U32 cmdBufferCount,
                            common::Resource<Swapchain> swapchain) = 0;

        virtual common::Resource<Shader> createShader(common::Resource<FrameBuffer> framebuffer,
                                                      const swizzle::gfx::ShaderType type,
                                                      const ShaderAttributeList& attribs) = 0;
        virtual common::Resource<Shader> createShader(common::Resource<Swapchain> swapchain,
                                                      const swizzle::gfx::ShaderType type,
                                                      const ShaderAttributeList& attribs) = 0;

        virtual common::Resource<Material> createMaterial(common::Resource<Shader> shader, SamplerMode samplerMode) = 0;

        virtual void enablePipelineStatistics(SwBool enable) = 0;
        virtual void enableGpuTiming(SwBool enable) = 0;

    protected:
        virtual ~GfxDevice() {}
    };

} // namespace swizzle::gfx

/* Function Declaration */

#endif