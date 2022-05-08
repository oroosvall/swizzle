
/* Include files */

#include "ShaderPipeline.hpp"

#include "Device.hpp"
#include "PresentFbo.hpp"
#include "VMaterial.hpp"

#include <glm/glm.hpp>

#include <fstream>
#include <swizzle/core/Platform.hpp>
#include <sstream>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

namespace vk
{
    // @ TODO: move to File handling api
    static std::string getPathFromFileName(const std::string& fileName);

    // @ TODO: move to shader helper functions
    static void readShaderInfo(const std::string& info, std::string& type, std::string& path);

    // @ TODO: move to shader helper functions
    static VkFormat getShaderAttributeFormat(swizzle::gfx::ShaderAttributeDataType type);
} // namespace swizzle::gfx

/* Static Function Definition */

namespace vk
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
    static VkFormat getShaderAttributeFormat(swizzle::gfx::ShaderAttributeDataType type)
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
} // namespace swizzle::gfx

/* Function Definition */

/* Class Public Function Definition */

namespace vk
{
    ShaderPipeline::ShaderPipeline(common::Resource<Device> device, const BaseFrameBuffer& frameBuffer,
                                   const swizzle::gfx::ShaderAttributeList& attribList)
        : mDevice(device)
        , mFrameBuffer(frameBuffer)
        , mShaderAttributes(attribList)
        , mPipelineLayout(VK_NULL_HANDLE)
        , mPipeline(VK_NULL_HANDLE)
        , mShaders()
        , mProperties()
        , mDescriptorLayout(VK_NULL_HANDLE)
    {
        mProperties[PropertyType::SourceBlend].mBlendFactor[0] = VkBlendFactor::VK_BLEND_FACTOR_ONE;
        mProperties[PropertyType::SourceBlend].mBlendFactor[1] = VkBlendFactor::VK_BLEND_FACTOR_ONE;
        mProperties[PropertyType::DestinationBlend].mBlendFactor[0] = VkBlendFactor::VK_BLEND_FACTOR_ONE;
        mProperties[PropertyType::DestinationBlend].mBlendFactor[1] = VkBlendFactor::VK_BLEND_FACTOR_ONE;
        mProperties[PropertyType::CullMode].mCullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
        mProperties[PropertyType::ColorBlendOp].mColorBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
        mProperties[PropertyType::AlphaBlendOp].mAlphaBlendOp = VkBlendOp::VK_BLEND_OP_ADD;

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
        layout_binding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;
        layout_binding[1].pImmutableSamplers = NULL;

        VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
        descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_layout.pNext = NULL;
        descriptor_layout.bindingCount = 2;
        descriptor_layout.pBindings = layout_binding;
        VkResult res = vkCreateDescriptorSetLayout(mDevice->getDeviceHandle(), &descriptor_layout, mDevice->getAllocCallbacks(),
                                    &mDescriptorLayout);
        vk::LogVulkanError(res, "vkCreateDescriptorSetLayout");

        VkPipelineLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.pNext = VK_NULL_HANDLE;
        info.flags = 0;
        info.setLayoutCount = 1;
        info.pSetLayouts = &mDescriptorLayout;
        info.pushConstantRangeCount = 1;
        info.pPushConstantRanges = &push;

        res = vkCreatePipelineLayout(mDevice->getDeviceHandle(), &info, mDevice->getAllocCallbacks(),
                               &mPipelineLayout);
        vk::LogVulkanError(res, "vkCreatePipelineLayout");
    }

    ShaderPipeline::~ShaderPipeline()
    {
        vkDestroyDescriptorSetLayout(mDevice->getDeviceHandle(), mDescriptorLayout,
                                     mDevice->getAllocCallbacks());

        vkDestroyPipeline(mDevice->getDeviceHandle(), mPipeline, mDevice->getAllocCallbacks());

        for (auto& it : mShaders)
        {
            vkDestroyShaderModule(mDevice->getDeviceHandle(), it.second, mDevice->getAllocCallbacks());
        }

        vkDestroyPipelineLayout(mDevice->getDeviceHandle(), mPipelineLayout, mDevice->getAllocCallbacks());
    }

    SwBool ShaderPipeline::load(const SwChar* file)
    {
        SwBool ok = false;

        std::ifstream inFile(file);

        if (inFile.is_open())
        {
            std::string line = "";

            enum class ParseState
            {
                PsNone,
                PsProp,
                PsVulkan
            };

            ParseState state = ParseState::PsNone;

            while (true)
            {
                std::getline(inFile, line);
                if (inFile.eof())
                {
                    break;
                }

                if (line == "[properties]")
                {
                    state = ParseState::PsProp;
                    continue;
                }
                else if (line == "[vulkan]")
                {
                    state = ParseState::PsVulkan;
                    continue;
                }
                else if (line[0] == '[')
                {
                    break;
                }
                else
                {
                    // Do Nothing
                }

                if (state == ParseState::PsProp)
                {
                    readProperty(line);
                }
                else if (state == ParseState::PsVulkan)
                {
                    std::string shaderType = "";
                    std::string path = "";

                    readShaderInfo(line, shaderType, path);
                    path.insert(0, getPathFromFileName(file));
                    loadShader(shaderType.c_str(), path.c_str());

                    LOG_INFO("Load shader [type=%s] from %s", shaderType.c_str(), path.c_str());
                }
            }

            createPipeline();
        }

        return ok;
    }

    SwBool ShaderPipeline::loadVertFragMemory(U32* vert, U32 vertSize, U32* frag, U32 fragSize, const SwChar* properties)
    {
        std::istringstream propertyString(properties);

        std::string line;
        while (std::getline(propertyString, line))
        {
            readProperty(line);
        }

        VkShaderModuleCreateInfo vertInfo = {};
        vertInfo.pCode = vert;
        vertInfo.codeSize = vertSize;
        vertInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vertInfo.pNext = VK_NULL_HANDLE;
        vertInfo.flags = 0;

        VkShaderModuleCreateInfo fragInfo = {};
        fragInfo.pCode = frag;
        fragInfo.codeSize = fragSize;
        fragInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fragInfo.pNext = VK_NULL_HANDLE;
        fragInfo.flags = 0;

        VkShaderModule vertModule = VK_NULL_HANDLE;
        VkShaderModule fragModule = VK_NULL_HANDLE;

        VkResult res = vkCreateShaderModule(mDevice->getDeviceHandle(), &vertInfo, mDevice->getAllocCallbacks(), &vertModule);
        vk::LogVulkanError(res, "vkCreateShaderModule");
        res = vkCreateShaderModule(mDevice->getDeviceHandle(), &fragInfo, mDevice->getAllocCallbacks(), &fragModule);
        vk::LogVulkanError(res, "vkCreateShaderModule");

        mShaders[ShaderModuleType::ShaderModuleType_Vertex] = vertModule;
        mShaders[ShaderModuleType::ShaderModuleType_Fragment] = fragModule;

        createPipeline();

        return true;
    }

    VkPipeline ShaderPipeline::getPipeline() const
    {
        return mPipeline;
    }

    VkPipelineLayout ShaderPipeline::getPipelineLayout() const
    {
        return mPipelineLayout;
    }

    VkDescriptorSetLayout ShaderPipeline::getDescriptorLayout() const
    {
        return mDescriptorLayout;
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{

    void ShaderPipeline::loadShader(const SwChar* shaderType, const SwChar* binaryFile)
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

            VkResult res = vkCreateShaderModule(mDevice->getDeviceHandle(), &info, mDevice->getAllocCallbacks(), &mod);
            vk::LogVulkanError(res, "vkCreateShaderModule");

            if (strcmp(shaderType, "vertex") == 0)
            {
                mShaders[ShaderModuleType::ShaderModuleType_Vertex] = mod;
            }
            else if (strcmp(shaderType, "geometry") == 0)
            {
                mShaders[ShaderModuleType::ShaderModuleType_Geometry] = mod;
            }
            else if (strcmp(shaderType, "fragment") == 0)
            {
                mShaders[ShaderModuleType::ShaderModuleType_Fragment] = mod;
            }
        }
    }

    void ShaderPipeline::createPipeline()
    {
        std::vector<VkPipelineShaderStageCreateInfo> shaderInfos;
        for (auto& it : mShaders)
        {
            VkShaderStageFlagBits stage = {};
            if (it.first == ShaderModuleType::ShaderModuleType_Vertex)
                stage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
            else if (it.first == ShaderModuleType::ShaderModuleType_Geometry)
                stage = VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
            else if (it.first == ShaderModuleType::ShaderModuleType_Fragment)
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
        int count = 0;
        std::vector<VkVertexInputBindingDescription> bindingDescriptors;
        bindingDescriptors.resize(mShaderAttributes.mBufferInput.size());
        for (auto it = mShaderAttributes.mBufferInput.begin(); it != mShaderAttributes.mBufferInput.end(); it++)
        {
            bindingDescriptors[count].binding = count;
            bindingDescriptors[count].inputRate = (it->mRate == swizzle::gfx::ShaderBufferInputRate::InputRate_Vertex
                                                       ? VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX
                                                       : VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE);
            bindingDescriptors[count].stride = it->mStride;
            count++;
        }

        count = 0;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptor;
        attributeDescriptor.resize(mShaderAttributes.mAttributes.size());
        for (auto it = mShaderAttributes.mAttributes.begin(); it != mShaderAttributes.mAttributes.end(); it++)
        {
            attributeDescriptor[count].location = count;
            attributeDescriptor[count].binding = it->mBufferIndex;
            attributeDescriptor[count].format = getShaderAttributeFormat(it->mDataType);
            attributeDescriptor[count].offset = it->mOffset;
            if (attributeDescriptor[count].format == VK_FORMAT_UNDEFINED)
            {
                swizzle::core::ShowCriticalMessage("Shader attribute is undefined format");
            }
            count++;
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
        if (mShaderAttributes.mPoints)
        {
            assemblyState.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        }
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
        rasterState.cullMode = mProperties[PropertyType::CullMode].mCullMode;
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
        depthState.depthTestEnable = mShaderAttributes.mEnableDepthTest;
        depthState.depthWriteEnable = mShaderAttributes.mEnableDepthTest;
        depthState.depthCompareOp =
            mShaderAttributes.mEnableDepthTest ? VkCompareOp::VK_COMPARE_OP_LESS : VkCompareOp::VK_COMPARE_OP_ALWAYS;
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
            blendState.blendEnable = mShaderAttributes.mEnableBlending;
            blendState.srcColorBlendFactor = mProperties[PropertyType::SourceBlend].mBlendFactor[0];
            blendState.srcAlphaBlendFactor = mProperties[PropertyType::SourceBlend].mBlendFactor[1];
            blendState.dstColorBlendFactor = mProperties[PropertyType::DestinationBlend].mBlendFactor[0];
            blendState.dstAlphaBlendFactor = mProperties[PropertyType::DestinationBlend].mBlendFactor[1];
            blendState.colorBlendOp = mProperties[PropertyType::ColorBlendOp].mColorBlendOp;
            blendState.alphaBlendOp = mProperties[PropertyType::AlphaBlendOp].mAlphaBlendOp;
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

        VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

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
        VkResult res = vkCreateGraphicsPipelines(mDevice->getDeviceHandle(), VK_NULL_HANDLE, 1u, &createInfo,
                                                 mDevice->getAllocCallbacks(), &mPipeline);
        if (res != VK_SUCCESS)
        {
            LOG_ERROR("Graphics pipeline creation failed %s", vk::VkResultToString(res));
        }
    }

    VkBlendFactor getBlendFactor(std::string& str)
    {
        if (str == "ONE")
        {
            return VkBlendFactor::VK_BLEND_FACTOR_ONE;
        }
        else if (str == "ZERO")
        {
            return VkBlendFactor::VK_BLEND_FACTOR_ZERO;
        }
        else if (str == "SRC_ALPHA")
        {
            return VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
        }
        else if (str == "SRC_COLOR")
        {
            return VkBlendFactor::VK_BLEND_FACTOR_SRC_COLOR;
        }
        else if (str == "DST_ALPHA")
        {
            return VkBlendFactor::VK_BLEND_FACTOR_DST_ALPHA;
        }
        else if (str == "DST_COLOR")
        {
            return VkBlendFactor::VK_BLEND_FACTOR_DST_COLOR;
        }
        else if (str == "ONE_MINUS_SRC_ALPHA")
        {
            return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        }
        else if (str == "ONE_MINUS_SRC_COLOR")
        {
            return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        }
        else if (str == "ONE_MINUS_DST_ALPHA")
        {
            return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        }
        else if (str == "ONE_MINUS_DST_COLOR")
        {
            return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        }
        else if (str == "ONE_MINUS_SRC_1_ALPHA")
        {
            return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        }
        else if (str == "ONE_MINUS_SRC_1_COLOR")
        {
            return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        }
        else if (str == "CONST_ALPHA")
        {
            return VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_ALPHA;
        }
        else if (str == "CONST_COLOR")
        {
            return VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_COLOR;
        }
        return VkBlendFactor::VK_BLEND_FACTOR_ONE;
    }

    void ShaderPipeline::readProperty(const std::string& line)
    {
        auto index = line.find('=');
        auto prop = line.substr(0, index);
        auto value = line.substr(index + 1);

        if (prop == "sourceBlend")
        {
            Property p = {};

            auto index2 = value.find(",");
            auto first = value.substr(1, index2 - 1);
            auto second = value.substr(index2 + 1);
            second.pop_back();

            p.mBlendFactor[0] = getBlendFactor(first);
            p.mBlendFactor[1] = getBlendFactor(second);

            mProperties[PropertyType::SourceBlend] = p;
        }
        else if (prop == "destinationBlend")
        {
            Property p = {};

            auto index2 = value.find(",");
            auto first = value.substr(1, index2 - 1);
            auto second = value.substr(index2 + 1);
            second.pop_back();

            p.mBlendFactor[0] = getBlendFactor(first);
            p.mBlendFactor[1] = getBlendFactor(second);

            mProperties[PropertyType::DestinationBlend] = p;
        }
        else if (prop == "cullMode")
        {
            if (value == "CULL_NONE")
            {
                mProperties[PropertyType::CullMode].mCullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
            }
            else if (value == "CULL_FRONT")
            {
                mProperties[PropertyType::CullMode].mCullMode = VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
            }
            else if (value == "CULL_BACK")
            {
                mProperties[PropertyType::CullMode].mCullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
            }
        }
        else if (prop == "alphaBlend")
        {
            if (value == "MULTIPLY")
            {
                mProperties[PropertyType::AlphaBlendOp].mAlphaBlendOp = VkBlendOp::VK_BLEND_OP_MULTIPLY_EXT;
            }
            else if (value == "ADD")
            {
                mProperties[PropertyType::AlphaBlendOp].mAlphaBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
            }
        }
        else if (prop == "colorBlend")
        {
            if (value == "MULTIPLY")
            {
                mProperties[PropertyType::ColorBlendOp].mColorBlendOp = VkBlendOp::VK_BLEND_OP_MULTIPLY_EXT;
            }
            else if (value == "ADD")
            {
                mProperties[PropertyType::ColorBlendOp].mColorBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
            }
        }
    }

} // namespace vk
