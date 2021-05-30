#ifndef COMMAND_BUFFER_HPP
#define COMMAND_BUFFER_HPP

#include <common/Common.hpp>

#include <swizzle/gfx/Swapchain.hpp>
#include <swizzle/gfx/FrameBuffer.hpp>
#include <swizzle/gfx/Buffer.hpp>
#include <swizzle/gfx/Texture.hpp>
#include <swizzle/gfx/Material.hpp>

namespace swizzle::gfx
{
    class CommandBuffer
    {
    public:
        virtual ~CommandBuffer() {}

        virtual U32 getDrawCount() = 0;
        virtual U64 getVertCount() = 0;
        virtual U64 getTriCount() = 0;

        virtual void reset(bool hardReset) = 0;

        virtual void begin() = 0;
        virtual void end() = 0;

        virtual void uploadTexture(common::Resource<Texture> texture) = 0;

        virtual void submit(common::Resource<Swapchain> swp = nullptr) = 0;

        virtual void beginRenderPass(common::Resource<Swapchain> swp) = 0;
        virtual void beginRenderPass(common::Resource<FrameBuffer> fbo) = 0;
        virtual void endRenderPass() = 0;

        virtual void bindShader(common::Resource<Shader> shader) = 0;
        virtual void bindMaterial(common::Resource<Shader> shader, common::Resource<Material> material) = 0;
        virtual void setShaderConstant(common::Resource<Shader> shader, U8* data, U32 size) = 0;
        virtual void setViewport(U32 x, U32 y) = 0;

        virtual void draw(common::Resource<Buffer> buffer) = 0;
        virtual void drawIndexed(common::Resource<Buffer> buffer, common::Resource<Buffer> index) = 0;

    };
}

#endif