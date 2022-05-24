#ifndef V_MATERIAL_HPP
#define V_MATERIAL_HPP

/* Include files */

#include <swizzle/gfx/Material.hpp>

#include "_fwDecl.hpp"

#include "../backend/Vk.hpp"
#include "VkResource.hpp"

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
        VMaterial(common::Resource<Device> device, common::Resource<swizzle::gfx::Shader> shader);
        virtual ~VMaterial();

        virtual U32 getNumDescriptors() const override;
        virtual swizzle::gfx::DescriptorType getDescriptorType(U32 index) override;

        virtual void setDescriptorBufferResource(U32 index, common::Resource<swizzle::gfx::Buffer> buffer,
            U64 size) override;
        virtual void setDescriptorTextureResource(U32 index, common::Resource<swizzle::gfx::Texture> texture) override;

        common::Resource<VkResource<VkDescriptorSet>> getDescriptorSet();
        void setDirty();

    private:

        void copyDescriptorIfDirty();

        common::Resource<Device> mDevice;
        common::Resource<swizzle::gfx::Shader> mShader;

        common::Resource<VkResource<VkDescriptorSet>> mDescriptorSet;

        // Temp
        VkSampler mSampler;
        SwBool mDirty;
    };


} // namespace swizzle::gfx

/* Function Declaration */

#endif