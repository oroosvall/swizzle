#ifndef SHADER_PIPELINE_HELPERS_HPP
#define SHADER_PIPELINE_HELPERS_HPP

/* Include files */

#include <swizzle/gfx/Shader.hpp>

#include "ShaderProperty.hpp"
#include "../../backend/Vk.hpp"

#include <vector>
#include <string>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace vk::shader
{
    VkPipelineVertexInputStateCreateInfo CreateVertexInput(std::vector<VkVertexInputBindingDescription>& bindingDescriptors, std::vector<VkVertexInputAttributeDescription>& attributeDescriptor);
    VkPipelineInputAssemblyStateCreateInfo CreateInputAssembly(VkPrimitiveTopology topology);
    VkPipelineTessellationStateCreateInfo CreateTesselationState();
    VkViewport CreateViewport(VkRect2D rect);
    VkPipelineViewportStateCreateInfo CreateViewportState(VkViewport* viewport, VkRect2D* rect);
    VkPipelineRasterizationStateCreateInfo CreateRasterizationState(VkCullModeFlags cullMode);
    VkPipelineMultisampleStateCreateInfo CreateMultisampleState(VkSampleCountFlagBits sampleCount);
    VkPipelineDepthStencilStateCreateInfo CreateDepthStencilState(SwBool depthTest, VkCompareOp depthCompare);

    void ReadShaderInfo(const std::string& info, std::string& type, std::string& path);
    VkFormat GetShaderAttributeFormat(swizzle::gfx::ShaderAttributeDataType type);
    VkDescriptorType GetDescriptorType(swizzle::gfx::DescriptorType type);
    VkShaderStageFlags GetStageFlags(common::IterType<swizzle::gfx::StageType>& types);
    SwBool GetShaderType(const SwChar* shaderType, shader::ShaderModuleType& type);


}

#endif
