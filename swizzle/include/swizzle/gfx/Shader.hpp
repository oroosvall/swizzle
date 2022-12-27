#ifndef SHADER_HPP
#define SHADER_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/Descriptor.hpp>
#include <swizzle/gfx/Material.hpp>

#include <vector>

/* Defines */

/* Typedefs/enums */

namespace swizzle::gfx
{
    enum class ShaderType
    {
        ShaderType_Graphics,
        ShaderType_Compute,
        ShaderType_Mesh
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

    enum class StageType
    {
        vertexStage = 0,
        tessellationStage = 1,
        meshStage = 2,
        taskStage = 3,
        fragmentStage = 4,
        computeStage,
    };

    enum class PrimitiveType
    {
        point = 0,
        line = 1,
        triangle = 2
    };

    typedef U32 Count;

}

/* Forward Declared Structs/Classes */

/* Struct/Class Declaration */

namespace swizzle::gfx
{
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

    class ShaderDescriptorBindings
    {
    public:
        ShaderDescriptorBindings(DescriptorType type, U32 descriptorCount, common::IterType<StageType> stages) : mType(type), mDescriptorCount(descriptorCount), mStages(stages) { }
        virtual ~ShaderDescriptorBindings() { }

        DescriptorType mType;
        U32 mDescriptorCount;
        common::IterType<StageType> mStages;
    };

    struct ShaderAttributeList
    {
        common::IterType<ShaderBufferInput> mBufferInput;
        common::IterType<ShaderAttribute> mAttributes;
        common::IterType<ShaderDescriptorBindings> mDescriptors;
        U32 mPushConstantSize;
        SwBool mEnableDepthTest;
        SwBool mEnableDepthWrite;
        SwBool mEnableBlending;
        PrimitiveType mPrimitiveType;
    };

    class Shader
    {
    public:
        virtual ~Shader() {}

        virtual SwBool load(const SwChar* file) = 0;
        virtual SwBool loadVertFragMemory(U32* vert, U32 vertSize, U32* frag, U32 fragSize, const SwChar* properties) = 0;

    };
}


/* Function Declaration */


#endif