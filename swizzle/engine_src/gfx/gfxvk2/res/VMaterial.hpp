#ifndef V_MATERIAL_HPP
#define V_MATERIAL_HPP

/* Include files */

#include <swizzle/gfx/Material.hpp>

#include "_fwDecl.hpp"

#include "../backend/Vk.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{

    class VMaterial : public swizzle::gfx::Material
    {
    public:
        VMaterial(common::Resource<Device> device, VkDescriptorSet descrSet);
        virtual ~VMaterial();

        virtual U32 getNumDescriptors() const override;
        virtual swizzle::gfx::MaterialDescriptorType getDescriptorType(U32 index) override;

        virtual void setDescriptorBufferResource(U32 index, common::Resource<swizzle::gfx::Buffer> buffer,
                                                 U64 size) override;
        virtual void setDescriptorTextureResource(U32 index, common::Resource<swizzle::gfx::Texture> texture) override;

        VkDescriptorSet getDescriptorSet();

    private:
        common::Resource<Device> mDevice;
        VkDescriptorSet mDescriptorSet;

        // Temp
        VkSampler mSampler;
    };
} // namespace swizzle::gfx

/* Function Declaration */

#endif