#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <common/Common.hpp>

#include <swizzle/gfx/Buffer.hpp>
#include <swizzle/gfx/Texture.hpp>

namespace swizzle::gfx
{
    class Shader;

    enum class MaterialDescriptorType
    {
        Invalid,
        TextureSampler,
        UniformBuffer,
    };

    class Material
    {
    public:

        virtual ~Material() {}

        virtual U32 getNumDescriptors() const = 0;
        virtual MaterialDescriptorType getDescriptorType(U32 index) = 0;

        virtual void setDescriptorBufferResource(U32 index, common::Resource<Buffer> buffer) = 0;
        virtual void setDescriptorTextureResource(U32 index, common::Resource<Texture> texture) = 0;

    };
}

#endif