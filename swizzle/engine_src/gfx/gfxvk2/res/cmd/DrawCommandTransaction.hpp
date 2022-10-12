#ifndef DRAW_COMMAND_TRANSACTION_HPP
#define DRAW_COMMAND_TRANSACTION_HPP

/* Include files */

#include "../../backend/Vk.hpp"
#include "../VkResource.hpp"
#include <swizzle/gfx/CommandBuffer.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{
    class VDrawCommandTransaction : public swizzle::gfx::DrawCommandTransaction
    {
    public:
        VDrawCommandTransaction(VkCommandBuffer commandBuffer, const common::Resource<LifetimeToken> token);
        virtual ~VDrawCommandTransaction();

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

        virtual void drawInstanced(common::Resource<swizzle::gfx::Buffer> buffer,
                                   common::Resource<swizzle::gfx::Buffer> instanceData) override;
        virtual void drawIndexedInstanced(common::Resource<swizzle::gfx::Buffer> buffer,
                                          common::Resource<swizzle::gfx::Buffer> index,
                                          common::Resource<swizzle::gfx::Buffer> instanceData) override;

        virtual void drawNoBind(U32 vertexCount, U32 first) override;
        virtual void drawIndexedNoBind(U32 vertexCount, U32 first, U32 vertOffset) override;

        virtual const swizzle::gfx::ScissorInfo pushScissorRegion(S32 x, S32 y, U32 w, U32 h) override;
        virtual void popScisorInfo(const swizzle::gfx::ScissorInfo& sci) override;

        virtual void setScissor(S32 x, S32 y, U32 w, U32 h) override;

        VkCommandBuffer getBuffer() const;
        const common::Resource<LifetimeToken> getToken() const;

    private:

        VkViewport makeViewport(F32 x, F32 y, F32 w, F32 h);
        VkRect2D makeRect2D(U32 w, U32 h);

        VkCommandBuffer mCommandBuffer;
        const common::Resource<LifetimeToken> mLifetimeToken;
        swizzle::gfx::ScissorInfo mScissorInfo;
    };
} // namespace vk

/* Function Declaration */

#endif
