#ifndef SHADER_HPP
#define SHADER_HPP

#include <common/Common.hpp>
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
        vec2f = 0,
        vec3f = 1,
        vec4f = 2,
        vec3i = 3,
        vec4i = 4,
        vec4u = 5,
        r8b8g8a8_unorm = 6,
    };

    class ShaderBufferInput
    {
    public:
        ShaderBufferInput() : mRate(ShaderBufferInputRate::InputRate_Vertex), mStride(0u) {}
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
        #ifdef SW_LINUX
        std::vector<ShaderBufferInput> mBufferInput;
        std::vector<ShaderAttribute> mAttributes;
        #else
        common::Iteratable<ShaderBufferInput> mBufferInput;
        common::Iteratable<ShaderAttribute> mAttributes;
        #endif
        SwBool mEnableDepthTest;
        SwBool mEnableBlending;
        SwBool mPoints;
    };

    class Shader
    {
    public:
        virtual ~Shader() {}

        virtual SwBool load(const SwChar* file) = 0;
        virtual SwBool loadVertFragMemory(U32* vert, U32 vertSize, U32* frag, U32 fragSize, const SwChar* properties) = 0;

    };
}

#endif