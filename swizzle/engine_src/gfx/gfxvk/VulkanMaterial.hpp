#ifndef VULKAN_MATERIAL_HPP
#define VULKAN_MATERIAL_HPP

#include <swizzle/gfx/Material.hpp>

#include <vector>
#include <memory>

namespace swizzle::gfx
{
    struct MaterialDescriptorInfo
    {
        MaterialDescriptorType mType;
        core::Resource<Buffer> mBufferResource;
        core::Resource<Texture> mTextureResource;
    };

    class VulkanMaterial : public Material
    {
    public:
        VulkanMaterial(Shader* shader);
        virtual ~VulkanMaterial();

        virtual U32 getNumDescriptors() const override;
        virtual MaterialDescriptorType getDescriptorType(U32 index) override;

        virtual void setDescriptorBufferResource(U32 index, core::Resource<Buffer> buffer) override;
        virtual void setDescriptorTextureResource(U32 index, core::Resource<Texture> texture) override;

    private:

        Shader* mWeakShaderPtr;

        std::vector<MaterialDescriptorInfo> mMaterialDescriptors;
    };
}

#endif