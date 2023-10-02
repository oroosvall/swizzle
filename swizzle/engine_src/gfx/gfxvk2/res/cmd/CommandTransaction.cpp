
/* Include files */

#include <swizzle/profiler/Profiler.hpp>

#include "../DBuffer.hpp"
#include "../ShaderPipeline.hpp"
#include "../TextureBase.hpp"
#include "../VMaterial.hpp"
#include "CommandTransaction.hpp"

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

    void VCommandTransaction::copyBuffer(common::Resource<swizzle::gfx::GfxBuffer> to,
                                         common::Resource<swizzle::gfx::GfxBuffer> from)
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

    void VCommandTransaction::copyBufferToTexture(common::Resource<swizzle::gfx::Texture> to,
                                                  common::Resource<swizzle::gfx::GfxBuffer> from,
                                                  const swizzle::gfx::TextureDimensions& size)
    {
        DBuffer* source = (DBuffer*)from.get();
        TextureBase* destination = (TextureBase*)to.get();

        destination->resize(size.mHeight, size.mWidth, 4u);
        const auto size2 = destination->getSize();

        auto& resSrc = source->getBuffer();
        auto resDst = destination->getImg();

        resSrc->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());
        resDst->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());

        VkBuffer srcBuf = resSrc->getVkHandle();
        VkImage dstImg = resDst->getVkHandle();

        vk::uploadTexture(mCommandBuffer, srcBuf, dstImg, size2);
    }

    void VCommandTransaction::copyTextureToBuffer(common::Resource<swizzle::gfx::GfxBuffer> to,
                                                  common::Resource<swizzle::gfx::Texture> from,
                                                  const swizzle::gfx::TextureDimensions& size)
    {
        TextureBase* source = (TextureBase*)from.get();
        DBuffer* destination = (DBuffer*)to.get();

        auto resSrc = source->getImg();
        auto& resDst = destination->getBuffer();

        resSrc->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());
        resDst->addUser(mLifetimeToken, mLifetimeToken->getCheckpoint());

        VkBuffer dstBuf = resDst->getVkHandle();
        VkImage srcImg = resSrc->getVkHandle();

        VkBufferImageCopy cpy{};
        cpy.bufferOffset = 0u;
        cpy.bufferImageHeight = size.mHeight;
        cpy.bufferRowLength = size.mWidth;
        cpy.imageOffset = {0, 0, 0};
        cpy.imageExtent.height = size.mHeight;
        cpy.imageExtent.width = size.mWidth;
        cpy.imageExtent.depth = 1;
        cpy.imageSubresource.baseArrayLayer = 0;
        cpy.imageSubresource.layerCount = 1;
        cpy.imageSubresource.mipLevel = 0;
        cpy.imageSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;

        VkImageMemoryBarrier imgBarrier = {};
        imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imgBarrier.pNext = VK_NULL_HANDLE;
        imgBarrier.srcAccessMask = 0;
        imgBarrier.dstAccessMask = 0;
        imgBarrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgBarrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.image = srcImg;
        imgBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgBarrier.subresourceRange.baseMipLevel = 0u;
        imgBarrier.subresourceRange.levelCount = 1;
        imgBarrier.subresourceRange.baseArrayLayer = 0u;
        imgBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(mCommandBuffer, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0u, VK_NULL_HANDLE, 0u,
                             VK_NULL_HANDLE, 1u, &imgBarrier);

        vkCmdCopyImageToBuffer(mCommandBuffer, srcImg, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstBuf, 1u,
                               &cpy);

        imgBarrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgBarrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

        vkCmdPipelineBarrier(mCommandBuffer, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0u, VK_NULL_HANDLE, 0u,
                             VK_NULL_HANDLE, 1u, &imgBarrier);
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

    void VCommandTransaction::hostBarrier()
    {
        VkMemoryBarrier memoryBarrier{};
        memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        memoryBarrier.pNext = VK_NULL_HANDLE;
        memoryBarrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_NONE;
        memoryBarrier.dstAccessMask = VkAccessFlagBits::VK_ACCESS_HOST_READ_BIT;

        vkCmdPipelineBarrier(mCommandBuffer, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_HOST_BIT,
                             VkPipelineStageFlagBits::VK_PIPELINE_STAGE_HOST_BIT, 0, 1u, &memoryBarrier, 0u,
                             VK_NULL_HANDLE, 0u, VK_NULL_HANDLE);
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
