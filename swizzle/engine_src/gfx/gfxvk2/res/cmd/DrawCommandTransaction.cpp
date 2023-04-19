
/* Include files */

#include "DrawCommandTransaction.hpp"
#include "../DBuffer.hpp"
#include "../ShaderPipeline.hpp"
#include "../TextureBase.hpp"
#include "../VMaterial.hpp"

#include <optick/optick.h>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Function Declaration */

/* Static Variables */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace vk
{
    VDrawCommandTransaction::VDrawCommandTransaction(VkCommandBuffer commandBuffer,
                                                     const common::Resource<LifetimeToken> token)
        : mCommandBuffer(commandBuffer)
        , mLifetimeToken(token)
        , mScissorInfo()
    {
    }

    VDrawCommandTransaction::~VDrawCommandTransaction() {}

    void VDrawCommandTransaction::bindShader(common::Resource<swizzle::gfx::Shader> shader)
    {
        ShaderPipeline* shad = (ShaderPipeline*)(shader.get());

        vkCmdBindPipeline(mCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, shad->getPipeline());
    }

    void VDrawCommandTransaction::bindMaterial(common::Resource<swizzle::gfx::Shader> shader,
                                               common::Resource<swizzle::gfx::Material> material)
    {
        OPTICK_EVENT("CmdBuffer::bindMaterial");
        ShaderPipeline* shad = (ShaderPipeline*)(shader.get());
        VMaterial* mat = (VMaterial*)(material.get());
        mat->setDirty();

        common::Resource<VkResource<VkDescriptorSet>> descSet = mat->getDescriptorSet();
        descSet->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());
        VkDescriptorSet descriptorHandle = descSet->getVkHandle();

        vkCmdBindDescriptorSets(mCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
                                shad->getPipelineLayout(), 0u, 1u, &descriptorHandle, 0u, VK_NULL_HANDLE);
    }

    void VDrawCommandTransaction::setShaderConstant(common::Resource<swizzle::gfx::Shader> shader, U8* data, U32 size)
    {
        OPTICK_EVENT("CmdBuffer::setShaderConstant");
        ShaderPipeline* shad = (ShaderPipeline*)(shader.get());

        vkCmdPushConstants(mCommandBuffer, shad->getPipelineLayout(), VK_SHADER_STAGE_ALL, 0u, size, data);
    }

    void VDrawCommandTransaction::setViewport(U32 x, U32 y)
    {
        OPTICK_EVENT("CmdBuffer::setViewport");
        // This ensures that the region is valid, 0, 0 is invalid size
        if (x == 0u || y == 0u)
        {
            x = 1u;
            y = 1u;
        }

        VkViewport vp = makeViewport(0.0f, static_cast<F32>(y), static_cast<F32>(x), -static_cast<F32>(y));
        VkRect2D r = makeRect2D(x, y);

        mScissorInfo.x = 0;
        mScissorInfo.y = 0;
        mScissorInfo.w = x;
        mScissorInfo.h = y;

        vkCmdSetViewport(mCommandBuffer, 0u, 1u, &vp);
        vkCmdSetScissor(mCommandBuffer, 0u, 1u, &r);
    }

    void VDrawCommandTransaction::enableStencilTest(SwBool enable)
    {
        vkCmdSetStencilTestEnable(mCommandBuffer, enable);
        vkCmdSetStencilReference(mCommandBuffer, VkStencilFaceFlagBits::VK_STENCIL_FRONT_AND_BACK, 0);
    }

    void VDrawCommandTransaction::setStencilReference(U8 reference)
    {
        vkCmdSetStencilReference(mCommandBuffer, VkStencilFaceFlagBits::VK_STENCIL_FRONT_AND_BACK, reference);
    }

    void VDrawCommandTransaction::bindVertexBuffer(common::Resource<swizzle::gfx::Buffer> buffer)
    {
        OPTICK_EVENT("CmdBuffer::bindVertexBuffer");
        DBuffer* buff = (DBuffer*)buffer.get();

        auto& res = buff->getBuffer();
        res->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());

        VkBuffer vkBuf = res->getVkHandle();
        VkDeviceSize offset = 0u;

        vkCmdBindVertexBuffers(mCommandBuffer, 0u, 1u, &vkBuf, &offset);
    }

    void VDrawCommandTransaction::bindIndexBuffer(common::Resource<swizzle::gfx::Buffer> buffer, SwBool bitSize16)
    {
        OPTICK_EVENT("CmdBuffer::bindIndexBuffer");
        DBuffer* buff = (DBuffer*)buffer.get();

        auto& res = buff->getBuffer();
        res->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());

        VkBuffer idxBuf = res->getVkHandle();
        VkDeviceSize offset = 0u;

        VkIndexType size[] = {VkIndexType::VK_INDEX_TYPE_UINT32, VkIndexType::VK_INDEX_TYPE_UINT16};

        vkCmdBindIndexBuffer(mCommandBuffer, idxBuf, offset, size[bitSize16]);
    }

    void VDrawCommandTransaction::draw(common::Resource<swizzle::gfx::Buffer> buffer)
    {
        OPTICK_EVENT("CmdBuffer::draw");
        // @TODO: Track draw count
        // mDrawCount++;
        DBuffer* buff = (DBuffer*)buffer.get();

        auto& res = buff->getBuffer();
        res->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());

        VkBuffer vkBuf = res->getVkHandle();

        VkDeviceSize offset = 0u;

        vkCmdBindVertexBuffers(mCommandBuffer, 0u, 1u, &vkBuf, &offset);
        vkCmdDraw(mCommandBuffer, buff->getCount(), 1u, 0u, 0u);
    }

    void VDrawCommandTransaction::drawIndexed(common::Resource<swizzle::gfx::Buffer> buffer,
                                              common::Resource<swizzle::gfx::Buffer> index)
    {
        OPTICK_EVENT("CmdBuffer::drawIndexed");
        // @TODO: Track draw count
        // mDrawCount++;
        DBuffer* buff = (DBuffer*)buffer.get();
        DBuffer* idxBuff = (DBuffer*)index.get();

        auto& resVert = buff->getBuffer();
        auto& resIdx = idxBuff->getBuffer();

        resVert->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());
        resIdx->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());

        VkBuffer vertBuf = resVert->getVkHandle();
        VkBuffer idxBuf = resIdx->getVkHandle();

        VkDeviceSize offset = 0u;

        vkCmdBindVertexBuffers(mCommandBuffer, 0U, 1U, &vertBuf, &offset);
        vkCmdBindIndexBuffer(mCommandBuffer, idxBuf, offset, VkIndexType::VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(mCommandBuffer, idxBuff->getCount() * 3u, 1u, 0u, 0u, 0u);
    }

    void VDrawCommandTransaction::drawInstanced(common::Resource<swizzle::gfx::Buffer> buffer,
                                                common::Resource<swizzle::gfx::Buffer> instanceData)
    {

        OPTICK_EVENT("CmdBuffer::drawInstanced");
        // TODO: Track draw count
        // mDrawCount++;

        auto vertex = GetBufferAsDBuffer(buffer);
        auto instance = GetBufferAsDBuffer(instanceData);

        vertex->getBuffer()->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());
        instance->getBuffer()->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());

        VkBuffer vertexHandle = vertex->getBuffer()->getVkHandle();
        VkBuffer instanceHandle = instance->getBuffer()->getVkHandle();
        VkDeviceSize offset = 0u;

        vkCmdBindVertexBuffers(mCommandBuffer, 0u, 1u, &vertexHandle, &offset);
        vkCmdBindVertexBuffers(mCommandBuffer, 1u, 1u, &instanceHandle, &offset);
        vkCmdDraw(mCommandBuffer, vertex->getCount(), instance->getCount(), 0u, 0u);
    }

    void VDrawCommandTransaction::drawIndexedInstanced(common::Resource<swizzle::gfx::Buffer> buffer,
                                                       common::Resource<swizzle::gfx::Buffer> index,
                                                       common::Resource<swizzle::gfx::Buffer> instanceData)
    {
        OPTICK_EVENT("CmdBuffer::drawIndexed");
        // TODO: Track draw count
        // mDrawCount++;

        auto vertex = GetBufferAsDBuffer(buffer);
        auto idx = GetBufferAsDBuffer(index);
        auto instance = GetBufferAsDBuffer(instanceData);

        vertex->getBuffer()->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());
        idx->getBuffer()->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());
        instance->getBuffer()->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());

        VkBuffer vertBuf = vertex->getBuffer()->getVkHandle();
        VkBuffer idxBuf = idx->getBuffer()->getVkHandle();
        VkBuffer inst = instance->getBuffer()->getVkHandle();

        VkDeviceSize offset = 0u;

        vkCmdBindVertexBuffers(mCommandBuffer, 0u, 1u, &vertBuf, &offset);
        vkCmdBindVertexBuffers(mCommandBuffer, 1u, 1u, &inst, &offset);
        vkCmdBindIndexBuffer(mCommandBuffer, idxBuf, offset, VkIndexType::VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(mCommandBuffer, idx->getCount() * 3u, instance->getCount(), 0u, 0u, 0u);
    }

    void VDrawCommandTransaction::drawMultiBufferIndexedInstanced(common::Resource<swizzle::gfx::Buffer> buffer,
                                                                  common::Resource<swizzle::gfx::Buffer> buffer2,
                                                                  common::Resource<swizzle::gfx::Buffer> index,
                                                                  common::Resource<swizzle::gfx::Buffer> instanceData)
    {
        OPTICK_EVENT("CmdBuffer::drawMultiBufferIndexedInstanced");

        auto vertex = GetBufferAsDBuffer(buffer);
        auto vertex2 = GetBufferAsDBuffer(buffer2);
        auto idx = GetBufferAsDBuffer(index);
        auto instance = GetBufferAsDBuffer(instanceData);

        auto tokenCheckpoint = mLifetimeToken->getCheckpoint();
        vertex->getBuffer()->addUser(mLifetimeToken, tokenCheckpoint);
        vertex2->getBuffer()->addUser(mLifetimeToken, tokenCheckpoint);
        idx->getBuffer()->addUser(mLifetimeToken, tokenCheckpoint);
        instance->getBuffer()->addUser(mLifetimeToken, tokenCheckpoint);

        VkBuffer vertBuf = vertex->getBuffer()->getVkHandle();
        VkBuffer vertBuf2 = vertex2->getBuffer()->getVkHandle();
        VkBuffer idxBuf = idx->getBuffer()->getVkHandle();
        VkBuffer inst = instance->getBuffer()->getVkHandle();

        VkDeviceSize offset = 0u;

        vkCmdBindVertexBuffers(mCommandBuffer, 0u, 1u, &vertBuf, &offset);
        vkCmdBindVertexBuffers(mCommandBuffer, 1u, 1u, &vertBuf2, &offset);
        vkCmdBindVertexBuffers(mCommandBuffer, 2u, 1u, &inst, &offset);
        vkCmdBindIndexBuffer(mCommandBuffer, idxBuf, offset, VkIndexType::VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(mCommandBuffer, idx->getCount() * 3u, instance->getCount(), 0u, 0u, 0u);
    }

    void VDrawCommandTransaction::drawMeshlet(U32 meshletCount)
    {
        vkCmdDrawMeshTasksEXT(mCommandBuffer, meshletCount, 1, 1);
    }

    void VDrawCommandTransaction::drawNoBind(U32 vertexCount, U32 first)
    {
        OPTICK_EVENT("CmdBuffer::drawNoBind");
        // TODO: Track draw count
        // mDrawCount++;
        vkCmdDraw(mCommandBuffer, vertexCount, 1u, first, 0u);
    }

    void VDrawCommandTransaction::drawIndexedNoBind(U32 vertexCount, U32 first, U32 vertOffset)
    {
        OPTICK_EVENT("CmdBuffer::drawIndexedNoBind");
        // TODO: Track draw count
        // mDrawCount++;
        vkCmdDrawIndexed(mCommandBuffer, vertexCount, 1u, first, vertOffset, 0u);
    }

    const swizzle::gfx::ScissorInfo VDrawCommandTransaction::pushScissorRegion(S32 x, S32 y, U32 w, U32 h)
    {
        swizzle::gfx::ScissorInfo org = mScissorInfo;

        mScissorInfo.enableCtr++;
        mScissorInfo.x = x;
        mScissorInfo.y = y;
        mScissorInfo.w = w;
        mScissorInfo.h = h;

        VkRect2D scissor{x, y, (U32)w, (U32)h};
        vkCmdSetScissor(mCommandBuffer, 0u, 1u, &scissor);

        return org;
    }

    void VDrawCommandTransaction::popScisorInfo(const swizzle::gfx::ScissorInfo& sci)
    {
        if (mScissorInfo.enableCtr == sci.enableCtr + 1u)
        {
            mScissorInfo = sci;
            VkRect2D scissor{sci.x, sci.y, (U32)sci.w, (U32)sci.h};
            vkCmdSetScissor(mCommandBuffer, 0u, 1u, &scissor);
        }
    }

    void VDrawCommandTransaction::setScissor(S32 x, S32 y, U32 w, U32 h)
    {
        VkRect2D scissor{x, y, w, h};
        vkCmdSetScissor(mCommandBuffer, 0u, 1u, &scissor);
    }

    VkCommandBuffer VDrawCommandTransaction::getBuffer() const
    {
        return mCommandBuffer;
    }

    const common::Resource<LifetimeToken> VDrawCommandTransaction::getToken() const
    {
        return mLifetimeToken;
    }

} // namespace vk

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    VkViewport VDrawCommandTransaction::makeViewport(F32 x, F32 y, F32 w, F32 h)
    {
        VkViewport vp = {};
        vp.x = x;
        vp.y = y;
        vp.width = w;
        vp.height = h;
        vp.minDepth = 0.0f;
        vp.maxDepth = 1.0f;

        return vp;
    }

    VkRect2D VDrawCommandTransaction::makeRect2D(U32 w, U32 h)
    {
        VkRect2D rect = {};
        rect.extent.width = w;
        rect.extent.height = h;
        rect.offset.x = 0;
        rect.offset.y = 0;

        return rect;
    }
} // namespace vk
