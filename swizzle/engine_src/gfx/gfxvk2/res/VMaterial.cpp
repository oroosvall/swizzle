
/* Include files */

#include "VMaterial.hpp"

#include "Device.hpp"
#include "DBuffer.hpp"
#include "TextureBase.hpp"
#include "ShaderPipeline.hpp"

#include <memory>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace vk
{
    VMaterial::VMaterial(common::Resource<Device> device, common::Resource<swizzle::gfx::Shader> shader)
        : mDevice(device)
        , mShader(shader)
        , mDescriptorSet(nullptr)
        , mSampler(VK_NULL_HANDLE)
        , mDirty(false)
    {
        auto shad = std::dynamic_pointer_cast<ShaderPipeline>(shader);
        mDescriptorSet = mDevice->allocateDescriptorSet(shad);

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

        vkCreateSampler(mDevice->getDeviceHandle(), &samplerInfo, mDevice->getAllocCallbacks(),
                        &mSampler);
    }

    VMaterial::~VMaterial()
    {
        vkDestroySampler(mDevice->getDeviceHandle(), mSampler, mDevice->getAllocCallbacks());
        mSampler = VK_NULL_HANDLE;
        mDevice->sheduleResourceDestruction(mDescriptorSet);
    }

    U32 VMaterial::getNumDescriptors() const
    {
        return 0;
    }

    swizzle::gfx::MaterialDescriptorType VMaterial::getDescriptorType(U32 index)
    {
        (void)index;
        return swizzle::gfx::MaterialDescriptorType::Invalid;
    }

    void VMaterial::setDescriptorBufferResource(U32 index, common::Resource<swizzle::gfx::Buffer> buffer, U64 size)
    {
        DBuffer* bfr = (DBuffer*)buffer.get();

        VkDescriptorBufferInfo descBfr = {};
        descBfr.buffer = bfr->getBuffer()->getVkHandle();
        descBfr.offset = 0;
        descBfr.range = size;

        VkWriteDescriptorSet writeDesc = {};

        writeDesc.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDesc.pNext = VK_NULL_HANDLE;
        writeDesc.dstSet = mDescriptorSet->getVkHandle();
        writeDesc.dstBinding = index;
        writeDesc.dstArrayElement = 0;
        writeDesc.descriptorCount = 1;
        writeDesc.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDesc.pImageInfo = VK_NULL_HANDLE;
        writeDesc.pBufferInfo = &descBfr;
        writeDesc.pTexelBufferView = VK_NULL_HANDLE;

        vkUpdateDescriptorSets(mDevice->getDeviceHandle(), 1U, &writeDesc, 0U, VK_NULL_HANDLE);
    }

    void VMaterial::setDescriptorTextureResource(U32 index, common::Resource<swizzle::gfx::Texture> texture)
    {
        TextureBase* tex = (TextureBase*)(texture.get());

        VkDescriptorImageInfo descImg = {};
        descImg.sampler = mSampler;
        descImg.imageView = tex->getView();
        descImg.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet writeDesc = {};

        writeDesc.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDesc.pNext = VK_NULL_HANDLE;
        writeDesc.dstSet = mDescriptorSet->getVkHandle();
        writeDesc.dstBinding = index;
        writeDesc.dstArrayElement = 0;
        writeDesc.descriptorCount = 1;
        writeDesc.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDesc.pImageInfo = &descImg;
        writeDesc.pBufferInfo = VK_NULL_HANDLE;
        writeDesc.pTexelBufferView = VK_NULL_HANDLE;

        vkUpdateDescriptorSets(mDevice->getDeviceHandle(), 1U, &writeDesc, 0U, VK_NULL_HANDLE);
    }

    common::Resource<VkResource<VkDescriptorSet>> VMaterial::getDescriptorSet()
    {
        return mDescriptorSet;
    }

    void VMaterial::setDirty()
    {
        mDirty = true;
    }

} // namespace swizzle::gfx

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    void VMaterial::copyDescriptor()
    {
        if (mDirty)
        {
            auto shad = std::dynamic_pointer_cast<ShaderPipeline>(mShader);
            auto newDescr = mDevice->allocateDescriptorSet(shad);

            VkCopyDescriptorSet cpy = {};
            cpy.sType = VkStructureType::VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
            cpy.pNext = VK_NULL_HANDLE;
            cpy.srcSet = mDescriptorSet->getVkHandle();
            cpy.srcBinding = 0;
            cpy.srcArrayElement = 0;
            cpy.dstSet = newDescr->getVkHandle();
            cpy.dstBinding = 0;
            cpy.dstArrayElement = 0;
            cpy.descriptorCount = 2;
            vkUpdateDescriptorSets(mDevice->getDeviceHandle(), 0, VK_NULL_HANDLE, 1, &cpy);

            mDevice->sheduleResourceDestruction(mDescriptorSet);
            mDescriptorSet = newDescr;

            mDirty = false;
        }
    }
} // namespace swizzle::gfx
