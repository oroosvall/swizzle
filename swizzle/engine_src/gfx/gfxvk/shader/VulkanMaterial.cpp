
/* Include files */

#include "../VulkanDevice.hpp"
#include "VulkanMaterial.hpp"

#include "../VulkanTexture.hpp"
#include "../VulkanBuffer.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace swizzle::gfx
{

    VulkanMaterial::VulkanMaterial(VkContainer vkObjects, VkDescriptorSet descrSet)
        : mVkObjects(vkObjects)
        , mDescriptorSet(descrSet)
    {

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.pNext = VK_NULL_HANDLE;
        samplerInfo.flags = 0;
        samplerInfo.magFilter = VkFilter::VK_FILTER_NEAREST;
        samplerInfo.minFilter = VkFilter::VK_FILTER_NEAREST;
        samplerInfo.mipmapMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
        samplerInfo.addressModeU = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.addressModeV = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.addressModeW = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.mipLodBias = 0.0F;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 0.0F;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VkCompareOp::VK_COMPARE_OP_ALWAYS;
        samplerInfo.minLod = 0.0F;
        samplerInfo.maxLod = 0.0F;
        samplerInfo.borderColor = VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        vkCreateSampler(mVkObjects.mLogicalDevice->getLogical(), &samplerInfo, mVkObjects.mDebugAllocCallbacks, &mSampler);

    }

    VulkanMaterial::~VulkanMaterial()
    {
        vkDestroySampler(mVkObjects.mLogicalDevice->getLogical(), mSampler, mVkObjects.mDebugAllocCallbacks);
        mSampler = VK_NULL_HANDLE;

        vkFreeDescriptorSets(mVkObjects.mLogicalDevice->getLogical(), mVkObjects.mDescriptorPool, 1, &mDescriptorSet);
        mDescriptorSet = VK_NULL_HANDLE;
    }

    U32 VulkanMaterial::getNumDescriptors() const
    {
        return 0;
    }

    MaterialDescriptorType VulkanMaterial::getDescriptorType(U32 index)
    {
        (void)index;
        return MaterialDescriptorType::Invalid;
    }

    void VulkanMaterial::setDescriptorBufferResource(U32 index, core::Resource<Buffer> buffer)
    {
        VulkanBuffer* bfr = (VulkanBuffer*)buffer.get();

        VkDescriptorBufferInfo descBfr = {};
        descBfr.buffer = bfr->getBuffer()->mBuffer;
        descBfr.offset = 0;
        descBfr.range = sizeof(float) * 4;

        VkWriteDescriptorSet writeDesc = {};

        writeDesc.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDesc.pNext = VK_NULL_HANDLE;
        writeDesc.dstSet = mDescriptorSet;
        writeDesc.dstBinding = index;
        writeDesc.dstArrayElement = 0;
        writeDesc.descriptorCount = 1;
        writeDesc.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDesc.pImageInfo = VK_NULL_HANDLE;
        writeDesc.pBufferInfo = &descBfr;
        writeDesc.pTexelBufferView = VK_NULL_HANDLE;

        vkUpdateDescriptorSets(mVkObjects.mLogicalDevice->getLogical(), 1U, &writeDesc, 0U, VK_NULL_HANDLE);
    }

    void VulkanMaterial::setDescriptorTextureResource(U32 index, core::Resource<Texture> texture)
    {
        VulkanTexture* tex = (VulkanTexture*)(texture.get());

        VkDescriptorImageInfo descImg = {};
        descImg.sampler = mSampler;
        descImg.imageView = tex->getView();
        descImg.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet writeDesc = {};

        writeDesc.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDesc.pNext = VK_NULL_HANDLE;
        writeDesc.dstSet = mDescriptorSet;
        writeDesc.dstBinding = index;
        writeDesc.dstArrayElement = 0;
        writeDesc.descriptorCount = 1;
        writeDesc.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDesc.pImageInfo = &descImg;
        writeDesc.pBufferInfo = VK_NULL_HANDLE;
        writeDesc.pTexelBufferView = VK_NULL_HANDLE;

        vkUpdateDescriptorSets(mVkObjects.mLogicalDevice->getLogical(), 1U, &writeDesc, 0U, VK_NULL_HANDLE);
    }

    VkDescriptorSet VulkanMaterial::getDescriptorSet()
    {
        return mDescriptorSet;
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */
