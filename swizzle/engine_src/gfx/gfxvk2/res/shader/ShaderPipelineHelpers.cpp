
/* Include files */

#include "ShaderPipelineHelpers.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace vk::shader
{
    VkPipelineVertexInputStateCreateInfo CreateVertexInput(std::vector<VkVertexInputBindingDescription>& bindingDescriptors, std::vector<VkVertexInputAttributeDescription>& attributeDescriptor)
    {
        VkPipelineVertexInputStateCreateInfo vertexInput = {};
        vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInput.pNext = VK_NULL_HANDLE;
        vertexInput.flags = 0;
        vertexInput.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptors.size());
        vertexInput.pVertexBindingDescriptions = bindingDescriptors.data();
        vertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptor.size());
        vertexInput.pVertexAttributeDescriptions = attributeDescriptor.data();

        return vertexInput;
    }

    VkPipelineInputAssemblyStateCreateInfo CreateInputAssembly(VkPrimitiveTopology topology)
    {
        VkPipelineInputAssemblyStateCreateInfo assemblyState = {};
        assemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        assemblyState.pNext = VK_NULL_HANDLE;
        assemblyState.flags = 0;
        assemblyState.topology = topology;
        assemblyState.primitiveRestartEnable = VK_FALSE;

        return assemblyState;
    }

    VkPipelineTessellationStateCreateInfo CreateTesselationState()
    {
        VkPipelineTessellationStateCreateInfo tessState = {};
        tessState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        tessState.pNext = VK_NULL_HANDLE;
        tessState.flags = 0;
        tessState.patchControlPoints = 0;

        return tessState;
    }

    VkViewport CreateViewport(VkRect2D rect)
    {
        VkViewport viewport = {};
        viewport.x = F32(rect.offset.x);
        viewport.y = F32(rect.offset.y);
        viewport.height = F32(rect.extent.height);
        viewport.width = F32(rect.extent.width);
        viewport.minDepth = 0.0F;
        viewport.maxDepth = 1.0F;

        return viewport;
    }

    VkPipelineViewportStateCreateInfo CreateViewportState(VkViewport* viewport, VkRect2D* rect)
    {
        VkPipelineViewportStateCreateInfo viewState = {};
        viewState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewState.pNext = VK_NULL_HANDLE;
        viewState.flags = 0;
        viewState.viewportCount = 1;
        viewState.pViewports = viewport;
        viewState.scissorCount = 1;
        viewState.pScissors = rect;

        return viewState;
    }

    VkPipelineRasterizationStateCreateInfo CreateRasterizationState(VkCullModeFlags cullMode)
    {
        VkPipelineRasterizationStateCreateInfo rasterState = {};
        rasterState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterState.pNext = VK_NULL_HANDLE;
        rasterState.flags = 0;
        rasterState.depthClampEnable = VK_FALSE;
        rasterState.rasterizerDiscardEnable = VK_FALSE;
        rasterState.polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
        rasterState.cullMode = cullMode;
        rasterState.frontFace = VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
        rasterState.depthBiasEnable = VK_FALSE;
        rasterState.depthBiasConstantFactor = 0.0F;
        rasterState.depthBiasClamp = 0.0F;
        rasterState.depthBiasSlopeFactor = 0.0F;
        rasterState.lineWidth = 1.0F;

        return rasterState;
    }

    VkPipelineMultisampleStateCreateInfo CreateMultisampleState(VkSampleCountFlagBits sampleCount)
    {
        VkPipelineMultisampleStateCreateInfo multisampleState = {};
        multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleState.pNext = VK_NULL_HANDLE;
        multisampleState.flags = 0;
        multisampleState.rasterizationSamples = sampleCount;
        multisampleState.sampleShadingEnable = VK_FALSE;
        multisampleState.minSampleShading = 1.0F;
        multisampleState.pSampleMask = VK_NULL_HANDLE;
        multisampleState.alphaToCoverageEnable = VK_FALSE;
        multisampleState.alphaToOneEnable = VK_FALSE;

        return multisampleState;
    }

    VkPipelineDepthStencilStateCreateInfo CreateDepthStencilState(SwBool depthTest, VkCompareOp depthCompare)
    {
        VkPipelineDepthStencilStateCreateInfo depthState = {};
        depthState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthState.pNext = VK_NULL_HANDLE;
        depthState.flags = 0;
        depthState.depthTestEnable = depthTest;
        depthState.depthWriteEnable = depthTest;
        depthState.depthCompareOp = depthCompare;
        depthState.depthBoundsTestEnable = VK_FALSE;
        depthState.stencilTestEnable = VK_FALSE;
        depthState.front = {};
        depthState.back = {};
        depthState.minDepthBounds = 0.0F;
        depthState.maxDepthBounds = 1.0F;

        return depthState;
    }

    void ReadShaderInfo(const std::string& info, std::string& type, std::string& path)
    {
        auto index = info.find('=');
        type = info.substr(0, index);
        path = info.substr(index + 1);
    }

    VkFormat GetShaderAttributeFormat(swizzle::gfx::ShaderAttributeDataType type)
    {
        VkFormat fmt = VK_FORMAT_UNDEFINED;
        switch (type)
        {
        case swizzle::gfx::ShaderAttributeDataType::vec2f:
            fmt = VK_FORMAT_R32G32_SFLOAT;
            break;
        case swizzle::gfx::ShaderAttributeDataType::vec3f:
            fmt = VK_FORMAT_R32G32B32_SFLOAT;
            break;
        case swizzle::gfx::ShaderAttributeDataType::vec4f:
            fmt = VK_FORMAT_R32G32B32A32_SFLOAT;
            break;
        case swizzle::gfx::ShaderAttributeDataType::vec3i:
            fmt = VK_FORMAT_R8G8B8_SINT;
            break;
        case swizzle::gfx::ShaderAttributeDataType::vec4i:
            fmt = VK_FORMAT_A8B8G8R8_SINT_PACK32;
            break;
        case swizzle::gfx::ShaderAttributeDataType::vec4u:
            // fmt = VK_FORMAT_A8B8G8R8_UINT_PACK32;
            fmt = VK_FORMAT_R32G32B32A32_UINT;
            break;
        case swizzle::gfx::ShaderAttributeDataType::r8b8g8a8_unorm:
            fmt = VK_FORMAT_R8G8B8A8_UNORM;
            break;
        default:
            break;
        }

        return fmt;
    }


    VkDescriptorType GetDescriptorType(swizzle::gfx::DescriptorType type)
    {
        VkDescriptorType ttype = VkDescriptorType::VK_DESCRIPTOR_TYPE_MAX_ENUM;

        switch (type)
        {
        case swizzle::gfx::DescriptorType::TextureSampler:
            ttype = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            break;
        case swizzle::gfx::DescriptorType::UniformBuffer:
            ttype = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
        case swizzle::gfx::DescriptorType::StorageBuffer:
            ttype = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            break;
        default:
            break;
        }

        return ttype;
    }

    VkShaderStageFlags GetStageFlags(swizzle::gfx::IterType<swizzle::gfx::StageType>& types)
    {
        VkShaderStageFlags flags = 0u;

        for (auto& it : types)
        {
            switch (it)
            {
            case swizzle::gfx::StageType::fragmentStage:
                flags |= VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            case swizzle::gfx::StageType::vertexStage:
                flags |= VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
                break;
            default:
                break;
            }
        }

        return flags;
    }

    SwBool GetShaderType(const SwChar* shaderType, shader::ShaderModuleType& type)
    {
        SwBool ok = true;
        if (strcmp(shaderType, "vertex") == 0)
        {
            type = shader::ShaderModuleType::ShaderModuleType_Vertex;
        }
        else if (strcmp(shaderType, "geometry") == 0)
        {
            type = shader::ShaderModuleType::ShaderModuleType_Geometry;
        }
        else if (strcmp(shaderType, "fragment") == 0)
        {
            type = shader::ShaderModuleType::ShaderModuleType_Fragment;
        }
        else
        {
            ok = false;
        }
        return ok;
    }

}

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
