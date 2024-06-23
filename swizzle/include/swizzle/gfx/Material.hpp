#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <common/Common.hpp>

#include <swizzle/gfx/Descriptor.hpp>
#include <swizzle/gfx/GfxBuffer.hpp>
#include <swizzle/gfx/Texture.hpp>

namespace swizzle::gfx
{
    class Shader;

    enum class SamplerMode
    {
        SamplerModeClamp,
        SamplerModeClampEdge,
        SamplerModeRepeat
    };

    class Material
    {
    public:

        virtual ~Material() {}

        virtual U32 getNumDescriptors() const = 0;
        virtual DescriptorType getDescriptorType(U32 index) = 0;
        virtual void useMipLevels(SwBool enable, SwBool forceLowest) = 0;

        virtual void setDescriptorBufferResource(U32 index, common::Resource<GfxBuffer> buffer, U64 size) = 0;
        virtual void setDescriptorTextureResource(U32 index, common::Resource<Texture> texture) = 0;
        virtual void setDescriptorComputeImageResource(U32 index, common::Resource<swizzle::gfx::Texture> texture) = 0;

    };
}

#endif