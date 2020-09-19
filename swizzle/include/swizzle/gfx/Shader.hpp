#ifndef SHADER_HPP
#define SHADER_HPP

#include <swizzle/core/common/Types.hpp>

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
        vec2_16,
        vec3_24,
        vec4_32,
        int3_24,
        int4_32,
    };

    struct ShaderBufferInput
    {
        ShaderBufferInputRate mRate;
        SwU32 mStride;
    };

    struct ShaderAttribute
    {
        // Buffer index to fetch data from
        SwU32 mBufferIndex;
        // data type determines size of attribute
        ShaderAttributeDataType mDataType;
        // offset from start of buffer
        SwU32 mOffset;
    };

    struct ShaderAttributeList
    {
        SwU32 mNumBuffers;
        ShaderBufferInput* mBufferInput;

        SwU32 mNumAttributes;
        ShaderAttribute* mAttributes;
    };



    class Shader
    {
    public:
        virtual ~Shader() {}

        virtual SwBool load(const SwChar* file) = 0;

    };
}

#endif