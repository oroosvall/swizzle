
/* Include files */

#include <swizzle/profiler/Profiler.hpp>

#include "CommandTransaction.hpp"
#include "../DBuffer.hpp"
#include "../ShaderPipeline.hpp"
#include "../TextureBase.hpp"
#include "../VMaterial.hpp"

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
    VCommandTransaction::VCommandTransaction(VkCommandBuffer commandBuffer, const common::Resource<LifetimeToken> token)
        : mCommandBuffer(commandBuffer)
        , mLifetimeToken(token)
    {
    }

    VCommandTransaction::~VCommandTransaction() {}

    void VCommandTransaction::copyBuffer(common::Resource<swizzle::gfx::Buffer> to,
                                         common::Resource<swizzle::gfx::Buffer> from)
    {
        DBuffer* source = (DBuffer*)from.get();
        DBuffer* destination = (DBuffer*)to.get();

        auto& resSrc = source->getBuffer();
        auto& resDst = destination->getBuffer();
        resSrc->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());
        resDst->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());

        VkBuffer srcBuf = resSrc->getVkHandle();
        VkBuffer dstBuf = resDst->getVkHandle();

        VkBufferCopy bufferCopy = {};

        bufferCopy.srcOffset = 0u;
        bufferCopy.dstOffset = 0u;
        bufferCopy.size = from->getUsedSize();
        const U32 regionCount = 1u;

        vkCmdCopyBuffer(mCommandBuffer, srcBuf, dstBuf, regionCount, &bufferCopy);
    }

    void VCommandTransaction::bindComputeShader(common::Resource<swizzle::gfx::Shader> shader,
                                                common::Resource<swizzle::gfx::Material> material, U8* constants,
                                                U32 constantsSize)
    {
        SWIZZLE_PROFILE_EVENT("CmdBuffer::bindComputeShader");

        ShaderPipeline* shad = (ShaderPipeline*)(shader.get());
        vkCmdBindPipeline(mCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, shad->getPipeline());

        if (material)
        {
            VMaterial* mat = (VMaterial*)(material.get());
            mat->setDirty();

            common::Resource<VkResource<VkDescriptorSet>> descSet = mat->getDescriptorSet();
            descSet->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());
            VkDescriptorSet descriptorHandle = descSet->getVkHandle();

            vkCmdBindDescriptorSets(mCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE,
                                    shad->getPipelineLayout(), 0u, 1u, &descriptorHandle, 0u, VK_NULL_HANDLE);
        }

        if (constants)
        {
            vkCmdPushConstants(mCommandBuffer, shad->getPipelineLayout(), VK_SHADER_STAGE_ALL, 0u, constantsSize,
                               constants);
        }
    }

    void VCommandTransaction::dispatchCompute(U32 groupX, U32 groupY, U32 groupZ)
    {
        vkCmdDispatch(mCommandBuffer, groupX, groupY, groupZ);
    }

    void VCommandTransaction::uploadTexture(common::Resource<swizzle::gfx::Texture> texture)
    {
        SWIZZLE_PROFILE_EVENT("CmdBuffer::uploadTexture");
        TextureBase* tex = (TextureBase*)(texture.get());
        if (!tex->isUploaded())
        {
            tex->uploadImage(mCommandBuffer);
        }
    }

    void VCommandTransaction::changeImageLayoutCompute(common::Resource<swizzle::gfx::Texture> texture)
    {
        SWIZZLE_PROFILE_EVENT("CmdBuffer::uploadTexture");
        TextureBase* tex = (TextureBase*)(texture.get());
        tex->transferImageToCompute(mCommandBuffer);
    }

    void VCommandTransaction::changeImageLayoutRender(common::Resource<swizzle::gfx::Texture> texture)
    {
        SWIZZLE_PROFILE_EVENT("CmdBuffer::uploadTexture");
        TextureBase* tex = (TextureBase*)(texture.get());
        tex->transferImageToRender(mCommandBuffer);
    }

    VkCommandBuffer VCommandTransaction::getBuffer() const
    {
        return mCommandBuffer;
    }

    const common::Resource<LifetimeToken> VCommandTransaction::getToken() const
    {
        return mLifetimeToken;
    }

} // namespace vk

/* Class Protected Function Definition */

/* Class Private Function Definition */
