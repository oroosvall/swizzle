
/* Include files */

#include "CommandTransaction.hpp"
#include "../DBuffer.hpp"
#include "../TextureBase.hpp"
#include "../ShaderPipeline.hpp"

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
        bufferCopy.size = from->getSize();
        const U32 regionCount = 1u;

        vkCmdCopyBuffer(mCommandBuffer, srcBuf, dstBuf, regionCount, &bufferCopy);
    }

    void VCommandTransaction::bindComputeShader(common::Resource<swizzle::gfx::Shader> shader)
    {
        ShaderPipeline* shad = (ShaderPipeline*)(shader.get());
        vkCmdBindPipeline(mCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, shad->getPipeline());
    }

    void VCommandTransaction::dispatchCompute(U32 groupX, U32 groupY, U32 groupZ)
    {
        vkCmdDispatch(mCommandBuffer, groupX, groupY, groupZ);
    }

    void VCommandTransaction::uploadTexture(common::Resource<swizzle::gfx::Texture> texture)
    {
        OPTICK_EVENT("CmdBuffer::uploadTexture");
        TextureBase* tex = (TextureBase*)(texture.get());
        if (!tex->isUploaded())
        {
            tex->uploadImage(mCommandBuffer);
        }
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
