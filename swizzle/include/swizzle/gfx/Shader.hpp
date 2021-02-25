#ifndef SHADER_HPP
#define SHADER_HPP

#include <common/Types.hpp>
#include <common/Resource.hpp>
#include <swizzle/gfx/Material.hpp>

#include <vector>

namespace swizzle::gfx
{
    enum class ShaderType
    {
        ShaderType_Graphics,
        ShaderType_Compute,
    };

    enum class ShaderBufferInputRate
    {
        InputRate_Vertex,
        InputRate_Instance,
    };

    enum class ShaderAttributeDataType
    {
        vec2_16 = 0,
        vec3_24 = 1,
        vec4_32 = 2,
        int3_24 = 3,
        int4_32 = 4,
    };

    class ShaderBufferInput
    {
    public:
        ShaderBufferInput(ShaderBufferInputRate rate, U32 stride) : mRate(rate), mStride(stride) {}
        virtual ~ShaderBufferInput() {}

        ShaderBufferInputRate mRate;
        U32 mStride;
    };

    class ShaderAttribute
    {
    public:
        ShaderAttribute(U32 index, ShaderAttributeDataType dataType, U32 offset) : mBufferIndex(index), mDataType(dataType), mOffset(offset) {}
        virtual ~ShaderAttribute() {}
        // Buffer index to fetch data from
        U32 mBufferIndex;
        // data type determines size of attribute
        ShaderAttributeDataType mDataType;
        // offset from start of buffer
        U32 mOffset;
    };

    struct ShaderAttributeList
    {
        U32 mNumBuffers;
        ShaderBufferInput* mBufferInput;
        U32 mNumAttributes;
        ShaderAttribute* mAttributes;
    };

    class Shader
    {
    public:
        virtual ~Shader() {}

        virtual SwBool load(const SwChar* file) = 0;
        virtual core::Resource<Material> createMaterial() = 0;



    };
}

#endif