
/* Include files */

#include <swizzle/gfx/Descriptor.hpp>

#include "VMaterial.hpp"

#include "DBuffer.hpp"
#include "Device.hpp"
#include "ShaderPipeline.hpp"
#include "TextureBase.hpp"

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
    VMaterial::VMaterial(common::Resource<Device> device, common::Resource<swizzle::gfx::Shader> shader,
                         swizzle::gfx::SamplerMode samplerMode)
        : mDevice(device)
        , mShader(shader)
        , mDescriptorSet(nullptr)
        , mSampler(VK_NULL_HANDLE)
        , mDirty(false)
        , mDescrTypes()
        , mSamplerMode(samplerMode)
    {
        auto shad = std::dynamic_pointer_cast<ShaderPipeline>(shader);
        mDescriptorSet = mDevice->allocateDescriptorSet(shad);
        const auto& attribs = shad->getAttributes();

        for (const auto& descr : attribs.mDescriptors)
        {
            mDescrTypes.push_back(descr.mType);
        }

        mSampler = createSampler(false, false);
        mSamplerMode = swizzle::gfx::SamplerMode::SamplerModeRepeat;
        mSampler2 = createSampler(false, false);
    }

    VMaterial::~VMaterial()
    {
        vkDestroySampler(mDevice->getDeviceHandle(), mSampler, mDevice->getAllocCallbacks());
        vkDestroySampler(mDevice->getDeviceHandle(), mSampler2, mDevice->getAllocCallbacks());
        mSampler = VK_NULL_HANDLE;
        mSampler2 = VK_NULL_HANDLE;
        mDevice->scheduleResourceDestruction(mDescriptorSet);
    }

    U32 VMaterial::getNumDescriptors() const
    {
        return static_cast<U32>(mDescrTypes.size());
    }

    swizzle::gfx::DescriptorType VMaterial::getDescriptorType(U32 index)
    {
        auto ret = swizzle::gfx::DescriptorType::Invalid;
        if (index < mDescrTypes.size())
        {
            ret = mDescrTypes[index];
        }
        return ret;
    }

    void VMaterial::useMipLevels(SwBool enable, SwBool forceLowest)
    {
        mDevice->waitDeviceIdle();
        vkDestroySampler(mDevice->getDeviceHandle(), mSampler, mDevice->getAllocCallbacks());
        mSampler = createSampler(enable, forceLowest);

        U32 index = 0u;
        for (const auto& descr : mDescrTypes)
        {
            if (descr == swizzle::gfx::DescriptorType::TextureSampler)
            {
                VkDescriptorImageInfo descImg = {};
                descImg.sampler = mSampler;
                descImg.imageView = VK_NULL_HANDLE;
                descImg.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;

                VkWriteDescriptorSet writeDesc = {};

                writeDesc.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDesc.pNext = VK_NULL_HANDLE;
                writeDesc.dstSet = mDescriptorSet->getVkHandle();
                writeDesc.dstBinding = index;
                writeDesc.dstArrayElement = 0u;
                writeDesc.descriptorCount = 1u;
                writeDesc.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;
                writeDesc.pImageInfo = &descImg;
                writeDesc.pBufferInfo = VK_NULL_HANDLE;
                writeDesc.pTexelBufferView = VK_NULL_HANDLE;

                vkUpdateDescriptorSets(mDevice->getDeviceHandle(), 1u, &writeDesc, 0u, VK_NULL_HANDLE);
            }
            index++;
        }
    }

    void VMaterial::setDescriptorBufferResource(U32 index, common::Resource<swizzle::gfx::GfxBuffer> buffer, U64 size)
    {
        copyDescriptorIfDirty(index);

        DBuffer* bfr = (DBuffer*)buffer.get();

        bfr->getBuffer()->addResourceDependency(mDescriptorSet);

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
        if (bfr->getType() == swizzle::gfx::GfxBufferType::UniformBuffer)
            writeDesc.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        else
            writeDesc.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        writeDesc.pImageInfo = VK_NULL_HANDLE;
        writeDesc.pBufferInfo = &descBfr;
        writeDesc.pTexelBufferView = VK_NULL_HANDLE;

        vkUpdateDescriptorSets(mDevice->getDeviceHandle(), 1U, &writeDesc, 0U, VK_NULL_HANDLE);
    }

    void VMaterial::setDescriptorTextureResource(U32 index, common::Resource<swizzle::gfx::Texture> texture)
    {
        copyDescriptorIfDirty(index);

        TextureBase* tex = (TextureBase*)(texture.get());

        tex->getImg()->addResourceDependency(mDescriptorSet);

        VkDescriptorImageInfo descImg = {};
        descImg.sampler = mSampler;
        if (index == 7u)
        {
            descImg.sampler = mSampler2;
        }
        descImg.imageView = tex->getView();
        descImg.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        if (tex->isDepth())
        {
            descImg.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
        }

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

    void VMaterial::setDescriptorComputeImageResource(U32 index, common::Resource<swizzle::gfx::Texture> texture)
    {
        copyDescriptorIfDirty(index);

        TextureBase* tex = (TextureBase*)(texture.get());

        tex->getImg()->addResourceDependency(mDescriptorSet);

        VkDescriptorImageInfo descImg = {};
        descImg.sampler = mSampler;
        descImg.imageView = tex->getView();
        descImg.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_GENERAL;
        if (tex->isDepth())
        {
            descImg.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
        }

        VkWriteDescriptorSet writeDesc = {};

        writeDesc.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDesc.pNext = VK_NULL_HANDLE;
        writeDesc.dstSet = mDescriptorSet->getVkHandle();
        writeDesc.dstBinding = index;
        writeDesc.dstArrayElement = 0;
        writeDesc.descriptorCount = 1;
        writeDesc.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
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

} // namespace vk

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    void VMaterial::copyDescriptorIfDirty(U32 idxToSkip)
    {
        if (mDirty)
        {
            auto shad = std::dynamic_pointer_cast<ShaderPipeline>(mShader);
            auto newDescr = mDevice->allocateDescriptorSet(shad);

            for (U32 i = 0u; i < mDescrTypes.size(); ++i)
            {
                if (i == idxToSkip)
                    continue;
                VkCopyDescriptorSet cpy = {};
                cpy.sType = VkStructureType::VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
                cpy.pNext = VK_NULL_HANDLE;
                cpy.srcSet = mDescriptorSet->getVkHandle();
                cpy.srcBinding = i;
                cpy.srcArrayElement = 0;
                cpy.dstSet = newDescr->getVkHandle();
                cpy.dstBinding = i;
                cpy.dstArrayElement = 0;
                cpy.descriptorCount = 1;

                vkUpdateDescriptorSets(mDevice->getDeviceHandle(), 0, VK_NULL_HANDLE, 1, &cpy);
            }

            mDevice->scheduleResourceDestruction(mDescriptorSet);
            mDescriptorSet = newDescr;

            mDirty = false;
        }
    }

    VkSampler VMaterial::createSampler(SwBool enableMips, SwBool forceLowestMip)
    {
        VkSampler sampler;
        VkSamplerAddressMode vkSamplerMode{};

        switch (mSamplerMode)
        {
        case swizzle::gfx::SamplerMode::SamplerModeClamp:
            vkSamplerMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            break;
        case swizzle::gfx::SamplerMode::SamplerModeClampEdge:
            vkSamplerMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            break;
        case swizzle::gfx::SamplerMode::SamplerModeRepeat:
            vkSamplerMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
            break;
        default:
            break;
        }

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.pNext = VK_NULL_HANDLE;
        samplerInfo.flags = 0;
        samplerInfo.magFilter = VkFilter::VK_FILTER_NEAREST;
        samplerInfo.minFilter = VkFilter::VK_FILTER_NEAREST;
        samplerInfo.mipmapMode = enableMips ? VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR
                                            : VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
        samplerInfo.addressModeU = vkSamplerMode;
        samplerInfo.addressModeV = vkSamplerMode;
        samplerInfo.addressModeW = vkSamplerMode;
        samplerInfo.mipLodBias = 0.0F;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 0.0F;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VkCompareOp::VK_COMPARE_OP_ALWAYS;
        samplerInfo.minLod = (enableMips && forceLowestMip) ? 4.0f : 0.0F;
        samplerInfo.maxLod = 8.0;
        samplerInfo.borderColor = VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        vkCreateSampler(mDevice->getDeviceHandle(), &samplerInfo, mDevice->getAllocCallbacks(), &sampler);

        return sampler;
    }
} // namespace vk
