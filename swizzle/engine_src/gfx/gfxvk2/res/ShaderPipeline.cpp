
/* Include files */

#include "ShaderPipeline.hpp"
#include "shader/ShaderPipelineHelpers.hpp"

#include "Device.hpp"
#include "PresentFbo.hpp"
#include "VMaterial.hpp"

#include <glm/glm.hpp>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <swizzle/core/Platform.hpp>
#include "core/FileUtils.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace vk
{
    ShaderPipeline::ShaderPipeline(common::Resource<Device> device, swizzle::gfx::ShaderType shaderType,
                                   common::Resource<vk::BaseFrameBuffer> frameBuffer,
                                   const swizzle::gfx::ShaderAttributeList& attribList)
        : mDevice(device)
        , mFrameBuffer(frameBuffer)
        , mShaderAttributes(attribList)
        , mShaderType(shaderType)
        , mPipelineLayout(VK_NULL_HANDLE)
        , mPipeline(VK_NULL_HANDLE)
        , mShaders()
        , mProperties()
        , mDescriptorLayout(VK_NULL_HANDLE)
    {
        mProperties[shader::PropertyType::SourceBlend].mBlendFactor[0] = VkBlendFactor::VK_BLEND_FACTOR_ONE;
        mProperties[shader::PropertyType::SourceBlend].mBlendFactor[1] = VkBlendFactor::VK_BLEND_FACTOR_ONE;
        mProperties[shader::PropertyType::DestinationBlend].mBlendFactor[0] = VkBlendFactor::VK_BLEND_FACTOR_ONE;
        mProperties[shader::PropertyType::DestinationBlend].mBlendFactor[1] = VkBlendFactor::VK_BLEND_FACTOR_ONE;
        mProperties[shader::PropertyType::CullMode].mCullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
        mProperties[shader::PropertyType::ColorBlendOp].mColorBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
        mProperties[shader::PropertyType::AlphaBlendOp].mAlphaBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
        mProperties[shader::PropertyType::StencilOp].mStencilOp[0] = VkStencilOp::VK_STENCIL_OP_KEEP;
        mProperties[shader::PropertyType::StencilOp].mStencilOp[1] = VkStencilOp::VK_STENCIL_OP_KEEP;
        mProperties[shader::PropertyType::StencilOp].mStencilOp[2] = VkStencilOp::VK_STENCIL_OP_KEEP;
        mProperties[shader::PropertyType::StencilEnable].mEnableStencilWrite = false;
        mProperties[shader::PropertyType::DepthCompare].mDepthCompare = VkCompareOp::VK_COMPARE_OP_LESS;
        mProperties[shader::PropertyType::WriteMask].mWriteMask =
            VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT |
            VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT;

        VkPushConstantRange push = {};

        push.size = mShaderAttributes.mPushConstantSize;
        push.offset = 0;
        push.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;

        std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
        layoutBindings.resize(mShaderAttributes.mDescriptors.size());

        int count = 0;
        for (auto it = mShaderAttributes.mDescriptors.begin(); it != mShaderAttributes.mDescriptors.end(); it++)
        {

            layoutBindings[count].binding = count;
            layoutBindings[count].descriptorType = shader::GetDescriptorType(it->mType);
            layoutBindings[count].descriptorCount = it->mDescriptorCount;
            auto stages = it->mStages;
            layoutBindings[count].stageFlags = shader::GetStageFlags(stages);
            layoutBindings[count].pImmutableSamplers = VK_NULL_HANDLE;

            count++;
        }

        VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
        descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_layout.pNext = NULL;
        descriptor_layout.bindingCount = (U32)layoutBindings.size();
        descriptor_layout.pBindings = layoutBindings.data();
        VkResult res = vkCreateDescriptorSetLayout(mDevice->getDeviceHandle(), &descriptor_layout,
                                                   mDevice->getAllocCallbacks(), &mDescriptorLayout);
        vk::LogVulkanError(res, "vkCreateDescriptorSetLayout");

        VkPipelineLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.pNext = VK_NULL_HANDLE;
        info.flags = 0;
        info.setLayoutCount = 1;
        info.pSetLayouts = &mDescriptorLayout;
        info.pushConstantRangeCount = (attribList.mPushConstantSize != 0) ? 1 : 0;
        info.pPushConstantRanges = &push;

        res = vkCreatePipelineLayout(mDevice->getDeviceHandle(), &info, mDevice->getAllocCallbacks(), &mPipelineLayout);
        vk::LogVulkanError(res, "vkCreatePipelineLayout");
    }

    ShaderPipeline::~ShaderPipeline()
    {
        vkDestroyDescriptorSetLayout(mDevice->getDeviceHandle(), mDescriptorLayout, mDevice->getAllocCallbacks());

        vkDestroyPipeline(mDevice->getDeviceHandle(), mPipeline, mDevice->getAllocCallbacks());

        for (auto& it : mShaders)
        {
            vkDestroyShaderModule(mDevice->getDeviceHandle(), it.second, mDevice->getAllocCallbacks());
        }

        vkDestroyPipelineLayout(mDevice->getDeviceHandle(), mPipelineLayout, mDevice->getAllocCallbacks());
    }

    SwBool ShaderPipeline::load(const SwChar* file)
    {
        SwBool ok = true;

        std::ifstream inFile(file);

        if (inFile.is_open())
        {
            std::string line = "";

            enum class ParseState
            {
                PsNone,
                PsSrc,
                PsProp,
                PsVulkan
            };

            ParseState state = ParseState::PsNone;

            while (true)
            {
                std::getline(inFile, line);
                line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
                if (inFile.eof())
                {
                    break;
                }

                if (line == "[properties]")
                {
                    state = ParseState::PsProp;
                    continue;
                }
                else if (line == "[src]")
                {
                    state = ParseState::PsSrc;
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
                    shader::Property prop;
                    shader::PropertyType type;
                    if (shader::ReadProperty(line, prop, type))
                    {
                        mProperties[type] = prop;
                    }
                }
                else if (state == ParseState::PsVulkan)
                {
                    std::string shaderType = "";
                    std::string path = "";

                    shader::ReadShaderInfo(line, shaderType, path);
                    path.insert(0, swizzle::core::getPathFromFileName(file));
                    shader::ShaderModuleType type = shader::ShaderModuleType::ShaderModuleType_Vertex;
                    ok = shader::GetShaderType(shaderType.c_str(), type);
                    if (ok)
                    {
                        LOG_INFO("Loading shader [type=%s] from %s", shaderType.c_str(), path.c_str());
                        ok = loadShader(type, path.c_str());
                    }
                }

                if (!ok)
                {
                    LOG_ERROR("Failed to load shader");
                    break;
                }
            }

            if (ok)
            {
                ok = createPipeline();
            }
        }

        return ok;
    }

    SwBool ShaderPipeline::loadVertFragMemory(U32* vert, U32 vertSize, U32* frag, U32 fragSize,
                                              const SwChar* properties)
    {
        SwBool ok = false;
        std::istringstream propertyString(properties);

        std::string line;
        while (std::getline(propertyString, line))
        {
            shader::Property prop;
            shader::PropertyType type;
            if (shader::ReadProperty(line, prop, type))
            {
                mProperties[type] = prop;
            }
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

        VkResult res =
            vkCreateShaderModule(mDevice->getDeviceHandle(), &vertInfo, mDevice->getAllocCallbacks(), &vertModule);
        vk::LogVulkanError(res, "vkCreateShaderModule");
        res = vkCreateShaderModule(mDevice->getDeviceHandle(), &fragInfo, mDevice->getAllocCallbacks(), &fragModule);
        vk::LogVulkanError(res, "vkCreateShaderModule");

        mShaders[shader::ShaderModuleType::ShaderModuleType_Vertex] = vertModule;
        mShaders[shader::ShaderModuleType::ShaderModuleType_Fragment] = fragModule;

        ok = createPipeline();

        return ok;
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

    const swizzle::gfx::ShaderAttributeList ShaderPipeline::getAttributes() const
    {
        return mShaderAttributes;
    }

    swizzle::gfx::ShaderType ShaderPipeline::getShaderType() const
    {
        return mShaderType;
    }

} // namespace vk

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    SwBool ShaderPipeline::loadShader(shader::ShaderModuleType shaderType, const SwChar* binaryFile)
    {
        SwBool ok = false;
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

            if (mod != VK_NULL_HANDLE)
            {
                mShaders[shaderType] = mod;
                ok = true;
            }
        }

        return ok;
    }

    SwBool ShaderPipeline::createPipeline()
    {
        std::vector<VkPipelineShaderStageCreateInfo> shaderInfos;
        createShaderInfo(shaderInfos);

        // Create bindings based on shader attributes
        std::vector<VkVertexInputBindingDescription> bindingDescriptors;
        createBindingDescriptions(bindingDescriptors);

        std::vector<VkVertexInputAttributeDescription> attributeDescriptor;
        createAttributeDescriptions(attributeDescriptor);

        // Create VertexInput based on binding/attribute descriptors
        VkPipelineVertexInputStateCreateInfo vertexInput =
            shader::CreateVertexInput(bindingDescriptors, attributeDescriptor);

        // Setup InputAssembly
        auto topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        if (mShaderAttributes.mPrimitiveType == swizzle::gfx::PrimitiveType::point)
        {
            topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        }
        else if (mShaderAttributes.mPrimitiveType == swizzle::gfx::PrimitiveType::line)
        {
            topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        }
        else if (mShaderAttributes.mPrimitiveType == swizzle::gfx::PrimitiveType::linelist)
        {
            topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        }
        if (mShaders.count(shader::ShaderModuleType::ShaderModuleType_TessellationControl))
        {
            topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
        }

        VkPipelineInputAssemblyStateCreateInfo assemblyState = shader::CreateInputAssembly(topology);

        // Setup TesselationState
        VkPipelineTessellationStateCreateInfo tessState = shader::CreateTesselationState();

        // Create rect for viewport/scissors
        VkRect2D r = {};
        r.extent.height = 100;
        r.extent.width = 100;
        r.offset.x = 0;
        r.offset.y = 0;

        // Setup Viewport
        VkViewport viewport = shader::CreateViewport(r);

        // Setup ViewportState
        VkPipelineViewportStateCreateInfo viewState = shader::CreateViewportState(&viewport, &r);

        // Setup RasterizationState
        VkPolygonMode polyMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
        // if (mShaders.count(shader::ShaderModuleType::ShaderModuleType_TesselationControl))
        //{
        //     polyMode = VkPolygonMode::VK_POLYGON_MODE_LINE;
        // }
        VkPipelineRasterizationStateCreateInfo rasterState =
            shader::CreateRasterizationState(mProperties[shader::PropertyType::CullMode].mCullMode, polyMode);

        // Setup MultisampleState
        VkPipelineMultisampleStateCreateInfo multiSampleState =
            shader::CreateMultisampleState(mFrameBuffer->getMultisampleCount());

        // Setup DepthStencilState
        VkPipelineDepthStencilStateCreateInfo depthState =
            shader::CreateDepthStencilState(mShaderAttributes.mEnableDepthTest, mShaderAttributes.mEnableDepthWrite,
                                            mProperties[shader::PropertyType::DepthCompare].mDepthCompare);

        VkStencilOpState state{};

        state.failOp = mProperties[shader::PropertyType::StencilOp].mStencilOp[0];
        state.passOp = mProperties[shader::PropertyType::StencilOp].mStencilOp[1];
        state.depthFailOp = mProperties[shader::PropertyType::StencilOp].mStencilOp[2];
        state.compareOp = mProperties[shader::PropertyType::StencilCompare].mStencilCompare;
        state.compareMask = mProperties[shader::PropertyType::StencilMask].mStencilMasks[0];
        state.writeMask = mProperties[shader::PropertyType::StencilMask].mStencilMasks[1];
        state.reference = mProperties[shader::PropertyType::StencilMask].mStencilMasks[2];

        depthState.back = state;
        depthState.front = state;
        depthState.stencilTestEnable = mProperties[shader::PropertyType::StencilEnable].mEnableStencilWrite;

        VkPipelineColorBlendStateCreateInfo colorState = {};
        colorState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorState.pNext = VK_NULL_HANDLE;
        colorState.flags = 0;
        colorState.logicOpEnable = VK_FALSE;
        colorState.logicOp = VkLogicOp::VK_LOGIC_OP_COPY;
        colorState.attachmentCount = mFrameBuffer->getNumColorAttachments(); // TODO: mBaseFbo->getNumAttachments();
        std::vector<VkPipelineColorBlendAttachmentState> attachs;
        for (size_t i = 0; i < colorState.attachmentCount; i++)
        {
            VkPipelineColorBlendAttachmentState blendState = {};
            blendState.blendEnable = mShaderAttributes.mEnableBlending;
            blendState.srcColorBlendFactor = mProperties[shader::PropertyType::SourceBlend].mBlendFactor[0];
            blendState.srcAlphaBlendFactor = mProperties[shader::PropertyType::SourceBlend].mBlendFactor[1];
            blendState.dstColorBlendFactor = mProperties[shader::PropertyType::DestinationBlend].mBlendFactor[0];
            blendState.dstAlphaBlendFactor = mProperties[shader::PropertyType::DestinationBlend].mBlendFactor[1];
            blendState.colorBlendOp = mProperties[shader::PropertyType::ColorBlendOp].mColorBlendOp;
            blendState.alphaBlendOp = mProperties[shader::PropertyType::AlphaBlendOp].mAlphaBlendOp;
            blendState.colorWriteMask = mProperties[shader::PropertyType::WriteMask].mWriteMask;

            attachs.push_back(blendState);
        }
        colorState.pAttachments = attachs.data();
        colorState.blendConstants[0] = 0.0F;
        colorState.blendConstants[1] = 0.0F;
        colorState.blendConstants[2] = 0.0F;
        colorState.blendConstants[3] = 0.0F;

        VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR,
                                          VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE, VK_DYNAMIC_STATE_STENCIL_REFERENCE};

        VkPipelineDynamicStateCreateInfo dynState = {};
        dynState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynState.pNext = VK_NULL_HANDLE;
        dynState.flags = 0;
        dynState.dynamicStateCount = COUNT_OF(dynamicStates);
        dynState.pDynamicStates = dynamicStates;

        // TODO: Add Shader cache
        VkResult res;

        if (mShaderType == swizzle::gfx::ShaderType::ShaderType_Graphics)
        {
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
            createInfo.renderPass = mFrameBuffer->getRenderPass();
            createInfo.subpass = 0U;
            createInfo.basePipelineHandle = VK_NULL_HANDLE;
            createInfo.basePipelineIndex = -1;

            res = vkCreateGraphicsPipelines(mDevice->getDeviceHandle(), VK_NULL_HANDLE, 1u, &createInfo,
                                            mDevice->getAllocCallbacks(), &mPipeline);
        }
        else if (mShaderType == swizzle::gfx::ShaderType::ShaderType_Mesh)
        {
            VkGraphicsPipelineCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            createInfo.pNext = VK_NULL_HANDLE;
            createInfo.flags = 0;
            createInfo.stageCount = static_cast<uint32_t>(shaderInfos.size());
            createInfo.pStages = shaderInfos.data();
            createInfo.pVertexInputState = VK_NULL_HANDLE;
            createInfo.pInputAssemblyState = VK_NULL_HANDLE;
            createInfo.pTessellationState = VK_NULL_HANDLE;
            createInfo.pViewportState = &viewState;
            createInfo.pRasterizationState = &rasterState;
            createInfo.pMultisampleState = &multiSampleState;
            createInfo.pDepthStencilState = &depthState;
            createInfo.pColorBlendState = &colorState;
            createInfo.pDynamicState = &dynState;
            createInfo.layout = mPipelineLayout;
            createInfo.renderPass = mFrameBuffer->getRenderPass();
            createInfo.subpass = 0U;
            createInfo.basePipelineHandle = VK_NULL_HANDLE;
            createInfo.basePipelineIndex = -1;

            res = vkCreateGraphicsPipelines(mDevice->getDeviceHandle(), VK_NULL_HANDLE, 1u, &createInfo,
                                            mDevice->getAllocCallbacks(), &mPipeline);
        }
        else
        {

            VkComputePipelineCreateInfo createInfo = {};

            createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
            createInfo.pNext = VK_NULL_HANDLE;
            createInfo.flags = 0u;
            createInfo.stage = shaderInfos[0];
            createInfo.layout = mPipelineLayout;
            createInfo.basePipelineHandle = VK_NULL_HANDLE;
            createInfo.basePipelineIndex = -1;

            res = vkCreateComputePipelines(mDevice->getDeviceHandle(), VK_NULL_HANDLE, 1u, &createInfo,
                                           mDevice->getAllocCallbacks(), &mPipeline);
        }

        if (res != VK_SUCCESS)
        {
            LOG_ERROR("Graphics pipeline creation failed %s", vk::VkResultToString(res));
        }

        return (mPipeline != nullptr);
    }

    void ShaderPipeline::createShaderInfo(std::vector<VkPipelineShaderStageCreateInfo>& shaderInfos)
    {
        for (auto& it : mShaders)
        {
            VkShaderStageFlagBits stage = {};
            if (it.first == shader::ShaderModuleType::ShaderModuleType_Vertex)
                stage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
            else if (it.first == shader::ShaderModuleType::ShaderModuleType_TessellationControl)
                stage = VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            else if (it.first == shader::ShaderModuleType::ShaderModuleType_TessellationEvaluate)
                stage = VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            else if (it.first == shader::ShaderModuleType::ShaderModuleType_Geometry)
                stage = VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
            else if (it.first == shader::ShaderModuleType::ShaderModuleType_Fragment)
                stage = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
            else if (it.first == shader::ShaderModuleType::ShaderModuleType_Compute)
                stage = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
            else if (it.first == shader::ShaderModuleType::ShaderModuleType_Task)
                stage = VkShaderStageFlagBits::VK_SHADER_STAGE_TASK_BIT_EXT;
            else if (it.first == shader::ShaderModuleType::ShaderModuleType_Mesh)
                stage = VkShaderStageFlagBits::VK_SHADER_STAGE_MESH_BIT_EXT;

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
    }

    void ShaderPipeline::createBindingDescriptions(std::vector<VkVertexInputBindingDescription>& bindingDescriptors)
    {
        S32 count = 0;
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
    }

    void
        ShaderPipeline::createAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptor)
    {
        S32 count = 0;
        attributeDescriptor.resize(mShaderAttributes.mAttributes.size());
        for (auto it = mShaderAttributes.mAttributes.begin(); it != mShaderAttributes.mAttributes.end(); it++)
        {
            attributeDescriptor[count].location = count;
            attributeDescriptor[count].binding = it->mBufferIndex;
            attributeDescriptor[count].format = shader::GetShaderAttributeFormat(it->mDataType);
            attributeDescriptor[count].offset = it->mOffset;
            if (attributeDescriptor[count].format == VK_FORMAT_UNDEFINED)
            {
                swizzle::core::ShowCriticalMessage("Shader attribute is undefined format");
            }
            count++;
        }
    }

} // namespace vk
