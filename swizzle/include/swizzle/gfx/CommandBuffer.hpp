#ifndef COMMAND_BUFFER_HPP
#define COMMAND_BUFFER_HPP

#include <swizzle/core/common/Types.hpp>
#include <swizzle/core/common/Resource.hpp>

#include <swizzle/gfx/Swapchain.hpp>
#include <swizzle/gfx/FrameBuffer.hpp>
#include <swizzle/gfx/Buffer.hpp>

namespace swizzle::gfx
{
    class CommandBuffer
    {
    public:
        virtual ~CommandBuffer() {}

        virtual void reset(bool hardReset) = 0;

        virtual void begin() = 0;
        virtual void end() = 0;

        virtual void submit(core::Resource<Swapchain> swp = nullptr) = 0;

        virtual void beginRenderPass(core::Resource<Swapchain> swp) = 0;
        virtual void beginRenderPass(core::Resource<FrameBuffer> fbo) = 0;
        virtual void endRenderPass() = 0;

        virtual void bindShader(core::Resource<Shader> shader) = 0;
        virtual void setShaderConstant(core::Resource<Shader> shader, SwU8* data, SwU32 size) = 0;
        virtual void setViewport(SwU32 x, SwU32 y) = 0;

        virtual void draw(core::Resource<Buffer> buffer) = 0;

    };
}

#endif