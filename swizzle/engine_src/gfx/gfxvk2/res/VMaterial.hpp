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
        VMaterial(common::Resource<Device> device, common::Resource<swizzle::gfx::Shader> shader, swizzle::gfx::SamplerMode samplerMode);
        virtual ~VMaterial();

        virtual U32 getNumDescriptors() const override;
        virtual swizzle::gfx::DescriptorType getDescriptorType(U32 index) override;

        virtual void useMipLevels(SwBool enable, SwBool forceLowest) override;

        virtual void setDescriptorBufferResource(U32 index, common::Resource<swizzle::gfx::GfxBuffer> buffer,
            U64 size) override;
        virtual void setDescriptorTextureResource(U32 index, common::Resource<swizzle::gfx::Texture> texture, SwBool copy) override;
        virtual void setDescriptorComputeImageResource(U32 index, common::Resource<swizzle::gfx::Texture> texture) override;

        common::Resource<VkResource<VkDescriptorSet>> getDescriptorSet();
        void setDirty();

    private:

        void copyDescriptorIfDirty();
        VkSampler createSampler(SwBool enableMips, SwBool forceLowestMip);

        common::Resource<Device> mDevice;
        common::Resource<swizzle::gfx::Shader> mShader;

        common::Resource<VkResource<VkDescriptorSet>> mDescriptorSet;

        // Temp
        VkSampler mSampler;
        VkSampler mSampler2;

        SwBool mDirty;

        std::vector<swizzle::gfx::DescriptorType> mDescrTypes;
        swizzle::gfx::SamplerMode mSamplerMode;

    };


} // namespace swizzle::gfx

/* Function Declaration */

#endif