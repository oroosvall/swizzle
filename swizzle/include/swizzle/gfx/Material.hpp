#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <common/Types.hpp>
#include <common/Resource.hpp>

#include <swizzle/gfx/Buffer.hpp>
#include <swizzle/gfx/Texture.hpp>

namespace swizzle::gfx
{
    class Shader;

    enum class MaterialDescriptorType
    {
        TextureSampler,
        UniformBuffer,
    };

    class Material
    {
    public:

        virtual U32 getNumDescriptors() const = 0;
        virtual MaterialDescriptorType getDescriptorType(U32 index) = 0;

        virtual void setDescriptorBufferResource(U32 index, core::Resource<Buffer> buffer) = 0;
        virtual void setDescriptorTextureResource(U32 index, core::Resource<Texture> texture) = 0;

    };
}

#endif