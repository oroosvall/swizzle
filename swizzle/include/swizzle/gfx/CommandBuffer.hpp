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
    struct ScissorInfo
    {
        U32 enableCtr;
        S32 x, y;
        S32 w, h;
    };

    class CommandBuffer : public std::enable_shared_from_this<CommandBuffer>
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

        virtual void beginRenderPass(common::Resource<Swapchain> swp) = 0;
        virtual void beginRenderPass(common::Resource<FrameBuffer> fbo) = 0;
        virtual void endRenderPass() = 0;

        virtual void bindShader(common::Resource<Shader> shader) = 0;
        virtual void bindMaterial(common::Resource<Shader> shader, common::Resource<Material> material) = 0;
        virtual void setShaderConstant(common::Resource<Shader> shader, U8* data, U32 size) = 0;
        virtual void setViewport(U32 x, U32 y) = 0;

        virtual void bindVertexBuffer(common::Resource<Buffer> buffer) = 0;
        virtual void bindIndexBuffer(common::Resource<Buffer> buffer, SwBool bitSize16) = 0;

        virtual void draw(common::Resource<Buffer> buffer) = 0;
        virtual void drawIndexed(common::Resource<Buffer> buffer, common::Resource<Buffer> index) = 0;

        virtual void drawInstanced(common::Resource<Buffer> buffer, common::Resource<Buffer> instanceData) = 0;
        virtual void drawIndexedInstanced(common::Resource<Buffer> buffer, common::Resource<Buffer> index, common::Resource<Buffer> instanceData) = 0;

        virtual void drawNoBind(U32 vertexCount, U32 first) = 0;
        virtual void drawIndexedNoBind(U32 vertexCount, U32 first, U32 vertOffset) = 0;

        virtual void dispatchCompute(U32 groupX, U32 groupY, U32 groupZ) = 0;

        virtual const ScissorInfo pushScissorRegion(S32 x, S32 y, S32 w, S32 h) = 0;
        virtual void popScisorInfo(const ScissorInfo& sci) = 0;

        virtual void setScissor(S32 x, S32 y, S32 w, S32 h) = 0;

        virtual void copyBuffer(common::Resource<Buffer> to, common::Resource<Buffer> from) = 0;

    };
}

#endif