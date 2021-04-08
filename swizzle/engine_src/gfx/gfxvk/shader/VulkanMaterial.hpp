#ifndef VULKAN_MATERIAL_HPP
#define VULKAN_MATERIAL_HPP

/* Include files */

#include "../backend/VkContainer.hpp"
#include <swizzle/gfx/Material.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::gfx
{

    class VulkanMaterial : public Material
    {
    public:

        VulkanMaterial(VkContainer vkObjects, VkDescriptorSet descrSet);
        virtual ~VulkanMaterial();

        virtual U32 getNumDescriptors() const override;
        virtual MaterialDescriptorType getDescriptorType(U32 index) override;

        virtual void setDescriptorBufferResource(U32 index, core::Resource<Buffer> buffer) override;
        virtual void setDescriptorTextureResource(U32 index, core::Resource<Texture> texture) override;

        VkDescriptorSet getDescriptorSet();

    private:

        VkContainer mVkObjects;
        VkDescriptorSet mDescriptorSet;

        // Temp
        VkSampler mSampler;

    };
}

/* Function Declaration */


#endif