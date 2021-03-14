/* Include files */

#include "../VulkanPhysicalDevice.hpp"

#include "VulkanShader.hpp"

#include <swizzle/core/Logging.hpp>

#include <fstream>
#include <string>

#include <glm/glm.hpp>

#include <vector>

/* Defines */

/* Typedefs */

/* Static Variables */

/* Static Function Declaration */

namespace swizzle::gfx
{
    // @ TODO: move to File handling api
    static std::string getPathFromFileName(const std::string& fileName);

    // @ TODO: move to shader helper functions
    static void readShaderInfo(const std::string& info, std::string& type, std::string& path);

    // @ TODO: move to shader helper functions
    static VkFormat getShaderAttributeFormat(ShaderAttributeDataType type);
}

/* Static Function Definition */

namespace swizzle::gfx
{
    // @ TODO: move to File handling api
    static std::string getPathFromFileName(const std::string& fileName)
    {
        std::size_t found = fileName.find_last_of("/\\");
        return fileName.substr(0, found + 1);
    }

    // @ TODO: move to shader helper functions
    static void readShaderInfo(const std::string& info, std::string& type, std::string& path)
    {
        auto index = info.find('=');
        type = info.substr(0, index);
        path = info.substr(index + 1);
    }

    // @ TODO: move to shader helper functions
    static VkFormat getShaderAttributeFormat(ShaderAttributeDataType type)
    {
        VkFormat fmt = VK_FORMAT_UNDEFINED;
        switch (type)
        {
        case ShaderAttributeDataType::vec2_16:
            fmt = VK_FORMAT_R32G32_SFLOAT;
            break;
        case ShaderAttributeDataType::vec3_24:
            fmt = VK_FORMAT_R32G32B32_SFLOAT;
            break;
        case ShaderAttributeDataType::vec4_32:
            fmt = VK_FORMAT_R32G32B32A32_SFLOAT;
            break;
        case ShaderAttributeDataType::int3_24:
            fmt = VK_FORMAT_R8G8B8_SINT;
            break;
        case ShaderAttributeDataType::int4_32:
            fmt = VK_FORMAT_A8B8G8R8_SINT_PACK32;
            break;
        default:
            break;
        }

        return fmt;
    }
}

/* Function Definition */

/* Class Public Function Definition */

namespace swizzle::gfx
{
    VulkanShader2::VulkanShader2(const VkContainer vkObjects, const PresentFBO& frameBuffer, const ShaderAttributeList& shaderAttributes)
        : mVkObjects(vkObjects)
        , mPipelineLayout(VK_NULL_HANDLE)
        , mPipeline(VK_NULL_HANDLE)
        , mFrameBuffer(frameBuffer)
        , mShaderAttributes(shaderAttributes)
    {
        VkPushConstantRange push = {};

        push.size = sizeof(glm::mat4) * 4;
        push.offset = 0;
        push.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;

        VkDescriptorSetLayoutBinding layout_binding[2] = {};
        layout_binding[0].binding = 0;
        layout_binding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        layout_binding[0].descriptorCount = 1;
        layout_binding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        layout_binding[0].pImmutableSamplers = NULL;

        layout_binding[1].binding = 1;
        layout_binding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layout_binding[1].descriptorCount = 1;
        layout_binding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        layout_binding[1].pImmutableSamplers = NULL;

        VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
        descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_layout.pNext = NULL;
        descriptor_layout.bindingCount = 2;
        descriptor_layout.pBindings = layout_binding;
        vkCreateDescriptorSetLayout(mVkObjects.mLogicalDevice->getLogical(), &descriptor_layout, NULL, &mDescriptorLayout);

        VkDescriptorSetAllocateInfo descAllocInfo = {};
        descAllocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descAllocInfo.pNext = VK_NULL_HANDLE;
        descAllocInfo.descriptorPool = mVkObjects.mDescriptorPool;
        descAllocInfo.descriptorSetCount = 1;
        descAllocInfo.pSetLayouts = &mDescriptorLayout;

        vkAllocateDescriptorSets(mVkObjects.mLogicalDevice->getLogical(), &descAllocInfo, &mDescriptorSet);

        VkPipelineLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.pNext = VK_NULL_HANDLE;
        info.flags = 0;
        info.setLayoutCount = 1;
        info.pSetLayouts = &mDescriptorLayout;
        info.pushConstantRangeCount = 1;
        info.pPushConstantRanges = &push;

        vkCreatePipelineLayout(mVkObjects.mLogicalDevice->getLogical(), &info, mVkObjects.mDebugAllocCallbacks, &mPipelineLayout);
    }

    VulkanShader2::~VulkanShader2()
    {
        //vkFreeDescriptorSets(mVkObjects.mLogicalDevice->getLogical(), mVkObjects.mDescriptorPool, 1, &mDescriptorSet); // TODO: Fixme

        vkDestroyDescriptorSetLayout(mVkObjects.mLogicalDevice->getLogical(), mDescriptorLayout, mVkObjects.mDebugAllocCallbacks);

        vkDestroyPipeline(mVkObjects.mLogicalDevice->getLogical(), mPipeline, mVkObjects.mDebugAllocCallbacks);

        for (auto& it : mShaders)
        {
            vkDestroyShaderModule(mVkObjects.mLogicalDevice->getLogical(), it.second, mVkObjects.mDebugAllocCallbacks);
        }

        vkDestroyPipelineLayout(mVkObjects.mLogicalDevice->getLogical(), mPipelineLayout, mVkObjects.mDebugAllocCallbacks);
    }

    SwBool VulkanShader2::load(const SwChar* filePath)
    {
        SwBool ok = false;

        std::ifstream inFile(filePath);

        if (inFile.is_open())
        {
            std::string backend = "";
            do
            {
                std::getline(inFile, backend);
            } while (backend != "[vulkan]");

            std::string info;

            do
            {
                std::string shaderType = "";
                std::string path = "";

                std::getline(inFile, info);
                if (inFile.eof() || info[0] == '[')
                    break;

                readShaderInfo(info, shaderType, path);
                path.insert(0, getPathFromFileName(filePath));
                loadShader(shaderType.c_str(), path.c_str());

                LOG_INFO("Load shader [type=%s] from %s", shaderType.c_str(), path.c_str());
            } while (true);

            createPipeline();
        }

        return ok;
    }

    core::Resource<Material> VulkanShader2::createMaterial()
    {
        return nullptr;
    }

    VkPipeline VulkanShader2::getPipeline() const
    {
        return mPipeline;
    }

    VkPipelineLayout VulkanShader2::getPipelineLayout() const
    {
        return mPipelineLayout;
    }

    VkDescriptorSet VulkanShader2::getDescriptorSet() const
    {
        return mDescriptorSet;
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace swizzle::gfx
{
    void VulkanShader2::loadShader(const SwChar* shaderType, const SwChar* binaryFile)
    {
        std::ifstream inFile(binaryFile, std::ios::binary | std::ios::ate);

        if (inFile.is_open())
        {
            std::vector<char> data;
            data.resize(inFile.tellg());
            inFile.seekg(0, std::ios::beg);
            inFile.read(data.data(), data.size());

            VkShaderModule mod = VK_NULL_HANDLE;
            VkShaderModuleCreateInfo info = {};
            info.pCode = (uint32_t*)data.data();
            info.codeSize = data.size();
            info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            info.pNext = VK_NULL_HANDLE;
            info.flags = 0;

            vkCreateShaderModule(mVkObjects.mLogicalDevice->getLogical(), &info, mVkObjects.mDebugAllocCallbacks, &mod);

            if (strcmp(shaderType, "vertex") == 0)
            {
                mShaders[ShaderModuleType::ShaderModuleType_Vertex] = mod;
            }
            else if (strcmp(shaderType, "fragment") == 0)
            {
                mShaders[ShaderModuleType::ShaderModuleType_Fragment] = mod;
            }
        }
    }

    void VulkanShader2::createPipeline()
    {
        std::vector<VkPipelineShaderStageCreateInfo> shaderInfos;
        for (auto& it : mShaders)
        {
            VkShaderStageFlagBits stage = {};
            if (it.first == ShaderModuleType::ShaderModuleType_Vertex)
                stage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
            if (it.first == ShaderModuleType::ShaderModuleType_Fragment)
                stage = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;

            VkPipelineShaderStageCreateInfo shInfo = {};
            shInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shInfo.pNext = VK_NULL_HANDLE;
            shInfo.flags = 0;
            shInfo.stage = stage;
            shInfo.module = it.second;
            shInfo.pName = "main";
            shInfo.pSpecializationInfo = VK_NULL_HANDLE;

            shaderInfos.push_back(shInfo);
        }

        // Create bindings based on shader attributes
        std::vector<VkVertexInputBindingDescription> bindingDescriptors;
        bindingDescriptors.resize(mShaderAttributes.mNumBuffers);
        for (U32 i = 0U; i < mShaderAttributes.mNumBuffers; ++i)
        {
            const ShaderBufferInput& input = mShaderAttributes.mBufferInput[i];
            bindingDescriptors[i].binding = i;
            bindingDescriptors[i].inputRate =
                (input.mRate == ShaderBufferInputRate::InputRate_Vertex ? VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX
                    : VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE);
            bindingDescriptors[i].stride = input.mStride;
        }

        std::vector<VkVertexInputAttributeDescription> attributeDescriptor;
        attributeDescriptor.resize(mShaderAttributes.mNumAttributes);
        for (U32 i = 0U; i < mShaderAttributes.mNumAttributes; ++i)
        {
            const ShaderAttribute& attrib = mShaderAttributes.mAttributes[i];
            attributeDescriptor[i].location = i;
            attributeDescriptor[i].binding = attrib.mBufferIndex;
            attributeDescriptor[i].format = getShaderAttributeFormat(attrib.mDataType);
            attributeDescriptor[i].offset = attrib.mOffset;
            if (attributeDescriptor[i].format == VK_FORMAT_UNDEFINED)
            {
                // TODO: move this to platform layer
                MessageBox(NULL, L"Format was undefined", L"Shader loading", MB_ICONERROR);
                //exit(-1);
            }
        }

        VkPipelineVertexInputStateCreateInfo vertexInput = {};
        vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInput.pNext = VK_NULL_HANDLE;
        vertexInput.flags = 0;
        vertexInput.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptors.size());
        vertexInput.pVertexBindingDescriptions = bindingDescriptors.data();
        vertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptor.size());
        vertexInput.pVertexAttributeDescriptions = attributeDescriptor.data();

        VkPipelineInputAssemblyStateCreateInfo assemblyState = {};
        assemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        assemblyState.pNext = VK_NULL_HANDLE;
        assemblyState.flags = 0;
        assemblyState.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        assemblyState.primitiveRestartEnable = VK_FALSE;

        VkPipelineTessellationStateCreateInfo tessState = {};
        tessState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        tessState.pNext = VK_NULL_HANDLE;
        tessState.flags = 0;
        tessState.patchControlPoints = 0;

        VkViewport viewport = {};
        viewport.x = 0;
        viewport.y = 0;
        viewport.height = 100;
        viewport.width = 100;
        viewport.minDepth = 0.0F;
        viewport.maxDepth = 1.0F;

        VkRect2D r = {};
        r.extent.height = 100;
        r.extent.width = 100;
        r.offset.x = 0;
        r.offset.y = 0;

        VkPipelineViewportStateCreateInfo viewState = {};
        viewState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewState.pNext = VK_NULL_HANDLE;
        viewState.flags = 0;
        viewState.viewportCount = 1;
        viewState.pViewports = &viewport;
        viewState.scissorCount = 1;
        viewState.pScissors = &r;

        VkPipelineRasterizationStateCreateInfo rasterState = {};
        rasterState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterState.pNext = VK_NULL_HANDLE;
        rasterState.flags = 0;
        rasterState.depthClampEnable = VK_FALSE;
        rasterState.rasterizerDiscardEnable = VK_FALSE;
        rasterState.polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
        rasterState.cullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
        rasterState.frontFace = VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
        rasterState.depthBiasEnable = VK_FALSE;
        rasterState.depthBiasConstantFactor = 0.0F;
        rasterState.depthBiasClamp = 0.0F;
        rasterState.depthBiasSlopeFactor = 0.0F;
        rasterState.lineWidth = 1.0F;

        VkPipelineMultisampleStateCreateInfo multiSampleState = {};
        multiSampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multiSampleState.pNext = VK_NULL_HANDLE;
        multiSampleState.flags = 0;
        multiSampleState.rasterizationSamples = mFrameBuffer.getMultisampleCount();
        multiSampleState.sampleShadingEnable = VK_FALSE;
        multiSampleState.minSampleShading = 1.0F;
        multiSampleState.pSampleMask = VK_NULL_HANDLE;
        multiSampleState.alphaToCoverageEnable = VK_FALSE;
        multiSampleState.alphaToOneEnable = VK_FALSE;

        VkPipelineDepthStencilStateCreateInfo depthState = {};
        depthState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthState.pNext = VK_NULL_HANDLE;
        depthState.flags = 0;
        depthState.depthTestEnable = VK_TRUE;
        depthState.depthWriteEnable = VK_TRUE;
        depthState.depthCompareOp = VkCompareOp::VK_COMPARE_OP_LESS;
        depthState.depthBoundsTestEnable = VK_FALSE;
        depthState.stencilTestEnable = VK_FALSE;
        depthState.front = {};
        depthState.back = {};
        depthState.minDepthBounds = 0.0F;
        depthState.maxDepthBounds = 1.0F;

        VkPipelineColorBlendStateCreateInfo colorState = {};
        colorState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorState.pNext = VK_NULL_HANDLE;
        colorState.flags = 0;
        colorState.logicOpEnable = VK_FALSE;
        colorState.logicOp = VkLogicOp::VK_LOGIC_OP_COPY;
        colorState.attachmentCount = mFrameBuffer.getNumColorAttachments(); // TODO: mBaseFbo->getNumAttachments();
        std::vector<VkPipelineColorBlendAttachmentState> attachs;
        for (size_t i = 0; i < colorState.attachmentCount; i++)
        {
            VkPipelineColorBlendAttachmentState blendState = {};
            blendState.blendEnable = VK_FALSE;
            blendState.srcAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
            blendState.dstColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
            blendState.dstAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
            blendState.srcColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
            blendState.colorBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
            blendState.alphaBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
            blendState.colorWriteMask = VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT |
                VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT |
                VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT |
                VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT;

            attachs.push_back(blendState);
        }
        colorState.pAttachments = attachs.data();
        colorState.blendConstants[0] = 0.0F;
        colorState.blendConstants[1] = 0.0F;
        colorState.blendConstants[2] = 0.0F;
        colorState.blendConstants[3] = 0.0F;

        VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

        VkPipelineDynamicStateCreateInfo dynState = {};
        dynState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynState.pNext = VK_NULL_HANDLE;
        dynState.flags = 0;
        dynState.dynamicStateCount = 2U;
        dynState.pDynamicStates = dynamicStates;

        VkGraphicsPipelineCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0;
        createInfo.stageCount = static_cast<uint32_t>(shaderInfos.size());
        createInfo.pStages = shaderInfos.data();
        createInfo.pVertexInputState = &vertexInput;
        createInfo.pInputAssemblyState = &assemblyState;
        createInfo.pTessellationState = &tessState;
        createInfo.pViewportState = &viewState;
        createInfo.pRasterizationState = &rasterState;
        createInfo.pMultisampleState = &multiSampleState;
        createInfo.pDepthStencilState = &depthState;
        createInfo.pColorBlendState = &colorState;
        createInfo.pDynamicState = &dynState;
        createInfo.layout = mPipelineLayout;
        createInfo.renderPass = mFrameBuffer.getRenderPass();
        createInfo.subpass = 0U;
        createInfo.basePipelineHandle = VK_NULL_HANDLE;
        createInfo.basePipelineIndex = -1;

        // TODO: Add Shader cache
        VkResult res = vkCreateGraphicsPipelines(mVkObjects.mLogicalDevice->getLogical(), VK_NULL_HANDLE, 1u, &createInfo, mVkObjects.mDebugAllocCallbacks,
            &mPipeline);
        if (res != VK_SUCCESS)
        {
            LOG_ERROR("Graphics pipeline creation failed %s", vk::VkResultToString(res));
        }
    }

} // namespace swizzle::gfx
