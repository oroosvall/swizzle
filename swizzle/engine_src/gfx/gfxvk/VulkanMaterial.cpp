#include "VulkanMaterial.hpp"

namespace swizzle::gfx
{

    VulkanMaterial::VulkanMaterial(Shader* shader)
        : mWeakShaderPtr(shader)
        , mMaterialDescriptors()
    {

    }

    VulkanMaterial::~VulkanMaterial()
    {

    }

    U32 VulkanMaterial::getNumDescriptors() const
    {
        return 0;
    }

    MaterialDescriptorType VulkanMaterial::getDescriptorType(U32 index)
    {
        return MaterialDescriptorType::TextureSampler;
    }

    void VulkanMaterial::setDescriptorBufferResource(U32 index, core::Resource<Buffer> buffer)
    {

    }

    void VulkanMaterial::setDescriptorTextureResource(U32 index, core::Resource<Texture> texture)
    {

    }
}