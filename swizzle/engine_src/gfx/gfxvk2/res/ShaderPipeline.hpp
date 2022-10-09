#ifndef SHADER_PIPELINE_HPP
#define SHADER_PIPELINE_HPP

/* Include files */

#include <swizzle/gfx/Shader.hpp>
#include "shader/ShaderProperty.hpp"
#include "shader/ShaderPropertyParser.hpp"

#include "_fwDecl.hpp"

#include "../backend/Vk.hpp"

#include <map>
#include <string>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{
    class ShaderPipeline : public swizzle::gfx::Shader
    {
    public:
        ShaderPipeline(common::Resource<Device> device, swizzle::gfx::ShaderType shaderType,
                       common::Resource<vk::BaseFrameBuffer> framebuffer,
                       const swizzle::gfx::ShaderAttributeList& attribList);
        virtual ~ShaderPipeline();

        virtual SwBool load(const SwChar* file) override;
        virtual SwBool loadVertFragMemory(U32* vert, U32 vertSize, U32* frag, U32 fragSize, const SwChar* properties) override;

        VkPipeline getPipeline() const;
        VkPipelineLayout getPipelineLayout() const;
        VkDescriptorSetLayout getDescriptorLayout() const;

        const swizzle::gfx::ShaderAttributeList getAttributes() const;

        swizzle::gfx::ShaderType getShaderType() const;

    private:

        SwBool loadShader(shader::ShaderModuleType, const SwChar* binaryFile);
        SwBool createPipeline();

        void createShaderInfo(std::vector<VkPipelineShaderStageCreateInfo>& shaderInfos);
        void createBindingDescriptions(std::vector<VkVertexInputBindingDescription>& bindingDescriptors);
        void createAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptor);

        common::Resource<Device> mDevice;
        common::Resource<vk::BaseFrameBuffer> mFrameBuffer;
        const swizzle::gfx::ShaderAttributeList mShaderAttributes;
        swizzle::gfx::ShaderType mShaderType;

        VkPipelineLayout mPipelineLayout;
        VkPipeline mPipeline;

        std::map<shader::ShaderModuleType, VkShaderModule> mShaders;
        std::map<shader::PropertyType, shader::Property> mProperties;

        VkDescriptorSetLayout mDescriptorLayout;

    };
}

/* Function Declaration */

#endif
