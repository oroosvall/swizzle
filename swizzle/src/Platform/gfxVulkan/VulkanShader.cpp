#include "VulkanShader.hpp"

#define SHADERC_SHAREDLIB
#define NV_EXTENSIONS
#include <shaderc/shaderc.h>

#include <string>
#include <fstream>

#include <iostream>

#include <glm/glm.hpp>

namespace swizzle
{

	VulkanShaderProgram::VulkanShaderProgram(VkDevice device, BaseFrameBuffer* baseFbo, VkPipelineCache pipelineCache, eShaderProgramType pipelineType)
		: mDevice(device)
		, mBaseFbo(baseFbo)
		, mCache(pipelineCache)
		, mPipelineType(pipelineType)
		, mPipelineLayout(VK_NULL_HANDLE)
		, mPipeline(VK_NULL_HANDLE)
	{

		VkPushConstantRange push;

		push.size = sizeof(glm::mat4);
		push.offset = 0;
		push.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;

		VkPipelineLayoutCreateInfo info;
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.pNext = VK_NULL_HANDLE;
		info.flags = 0;
		info.setLayoutCount = 0;
		info.pSetLayouts  = VK_NULL_HANDLE;
		info.pushConstantRangeCount = 1;
		info.pPushConstantRanges = &push;

		vkCreatePipelineLayout(mDevice, &info, nullptr, &mPipelineLayout);
	}

	VulkanShaderProgram::~VulkanShaderProgram()
	{
		vkDestroyPipeline(mDevice, mPipeline, nullptr);

		for (auto& it : mShaders)
		{
			vkDestroyShaderModule(mDevice, it.second, nullptr);
		}

		vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
	}

	eShaderProgramType VulkanShaderProgram::getProgramType() const
	{
		return mPipelineType;
	}

	bool VulkanShaderProgram::isShaderTypeSupported(ShaderType type) const
	{
		type;
		return true;
	}

	void readShaderInfo(const std::string& info, std::string& type, std::string& path)
	{
		auto index = info.find('=');
		type = info.substr(0, index);
		path = info.substr(index + 1);
	}

	std::string getPathFromFileName(const std::string& fileName)
	{
		std::size_t found = fileName.find_last_of("/\\");
		return fileName.substr(0, found + 1);
	}

	bool VulkanShaderProgram::load(const char* file)
	{
		bool ok = false;

		std::ifstream inFile(file);
		
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
				path.insert(0, getPathFromFileName(file));
				loadShader(shaderType.c_str(), path.c_str());

				printf("%s, %s\n", shaderType.c_str(), path.c_str());
			} while (true);
		
			createPipeline();

		}
		
		return ok;
	}

	void VulkanShaderProgram::compileShader(const char* sourceFile)
	{
		sourceFile;
	}

	void VulkanShaderProgram::compileShader(const char* source, uint32_t size)
	{
		source;
		size;
		//shaderc_compiler_t compiler = shaderc_compiler_initialize();
		//shaderc_compilation_result_t result = shaderc_compile_into_spv(
		//    compiler, source, size,
		//    shaderc_glsl_vertex_shader, "main.vert", "main", nullptr);
		//// Do stuff with compilation results.
		//
		//if (shaderc_result_get_compilation_status(result) == shaderc_compilation_status_success)
		//{
		//	
		//}

		//shaderc_result_release(result);
		//shaderc_compiler_release(compiler);

	}

	bool VulkanShaderProgram::isBinarySupported() const
	{
		return true;
	}

	void VulkanShaderProgram::loadShader(const char* shaderType, const char* binaryFile)
	{
		std::ifstream inFile(binaryFile, std::ios::binary | std::ios::ate);

		if (inFile.is_open())
		{
			std::vector<char> data;
			data.resize(inFile.tellg());
			inFile.seekg(0, std::ios::beg);
			inFile.read(data.data(), data.size());

			VkShaderModule mod = VK_NULL_HANDLE;
			VkShaderModuleCreateInfo info;
			info.pCode = (uint32_t*)data.data();
			info.codeSize = data.size();
			info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			info.pNext = VK_NULL_HANDLE;
			info.flags = 0;

			vkCreateShaderModule(mDevice, &info, nullptr, &mod);

			if (strcmp(shaderType, "vertex") == 0)
			{
				mShaders[shaderTypes::vertex] = mod;
			}
			else if (strcmp(shaderType, "fragment") == 0)
			{
				mShaders[shaderTypes::fragment] = mod;
			}

		}
	}

	void VulkanShaderProgram::loadShader(const char* binary, uint32_t size)
	{
		binary;
		size;
	}

	void VulkanShaderProgram::buildShaderProgram()
	{
	}

	void VulkanShaderProgram::createPipeline()
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderInfos;
		for (auto& it : mShaders)
		{
			VkShaderStageFlagBits stage{};
			if (it.first == shaderTypes::vertex)
				stage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
			if (it.first == shaderTypes::fragment)
				stage = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;

			VkPipelineShaderStageCreateInfo shInfo;
			shInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shInfo.pNext = VK_NULL_HANDLE;
			shInfo.flags = 0;
			shInfo.stage = stage;
			shInfo.module = it.second;
			shInfo.pName = "main";
			shInfo.pSpecializationInfo = VK_NULL_HANDLE;

			shaderInfos.push_back(shInfo);
		}
		
		VkVertexInputBindingDescription bindDescr;
		bindDescr.binding = 0;
		bindDescr.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
		bindDescr.stride = sizeof(float) * 3;

		VkVertexInputAttributeDescription attrDescr;
		attrDescr.binding = 0;
		attrDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
		attrDescr.location = 0;
		attrDescr.offset = 0;

		VkPipelineVertexInputStateCreateInfo vertexInput;
		vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInput.pNext = VK_NULL_HANDLE;
		vertexInput.flags = 0;
		vertexInput.vertexBindingDescriptionCount = 1;
		vertexInput.pVertexBindingDescriptions = &bindDescr;
		vertexInput.vertexAttributeDescriptionCount = 1;
		vertexInput.pVertexAttributeDescriptions = &attrDescr;

		VkPipelineInputAssemblyStateCreateInfo assemblyState;
		assemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		assemblyState.pNext = VK_NULL_HANDLE;
		assemblyState.flags = 0;
		assemblyState.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		assemblyState.primitiveRestartEnable = VK_FALSE;

		VkPipelineTessellationStateCreateInfo tessState;
		tessState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		tessState.pNext = VK_NULL_HANDLE;
		tessState.flags = 0;
		tessState.patchControlPoints = 0;

		VkViewport viewport;
		viewport.x = 0;
		viewport.y = 0;
		viewport.height = 100;
		viewport.width = 100;
		viewport.minDepth = 0.0F;
		viewport.maxDepth = 1.0F;

		VkRect2D r;
		r.extent.height = 100;
		r.extent.width = 100;
		r.offset.x = 0;
		r.offset.y= 0;

		VkPipelineViewportStateCreateInfo viewState;
		viewState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewState.pNext = VK_NULL_HANDLE;
		viewState.flags = 0;
		viewState.viewportCount = 1;
		viewState.pViewports = &viewport;
		viewState.scissorCount = 1;
		viewState.pScissors = &r;

		VkPipelineRasterizationStateCreateInfo rasterState;
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

		VkPipelineMultisampleStateCreateInfo multiSampleState;
		multiSampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multiSampleState.pNext = VK_NULL_HANDLE;
		multiSampleState.flags = 0;
		multiSampleState.rasterizationSamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		multiSampleState.sampleShadingEnable = VK_FALSE;
		multiSampleState.minSampleShading = 1.0F;
		multiSampleState.pSampleMask = VK_NULL_HANDLE;
		multiSampleState.alphaToCoverageEnable = VK_FALSE;
		multiSampleState.alphaToOneEnable = VK_FALSE;


		VkPipelineDepthStencilStateCreateInfo depthState;
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

		VkPipelineColorBlendStateCreateInfo colorState;
		colorState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorState.pNext = VK_NULL_HANDLE;
		colorState.flags = 0;
		colorState.logicOpEnable = VK_FALSE;
		colorState.logicOp = VkLogicOp::VK_LOGIC_OP_COPY;
		colorState.attachmentCount = mBaseFbo->getNumAttachments();
		std::vector<VkPipelineColorBlendAttachmentState> attachs;
		for (size_t i = 0; i < mBaseFbo->getNumAttachments(); i++)
		{
			VkPipelineColorBlendAttachmentState blendState;
			blendState.blendEnable = VK_FALSE;
			blendState.srcAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
			blendState.dstColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
			blendState.dstAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
			blendState.srcColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
			blendState.colorBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
			blendState.alphaBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
			blendState.colorWriteMask = VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT;

			attachs.push_back(blendState);
		}
		colorState.pAttachments = attachs.data();
		colorState.blendConstants[0] = 0.0F;
		colorState.blendConstants[1] = 0.0F;
		colorState.blendConstants[2] = 0.0F;
		colorState.blendConstants[3] = 0.0F;

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynState;
		dynState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynState.pNext = VK_NULL_HANDLE;
		dynState.flags = 0;
		dynState.dynamicStateCount = 2U;
		dynState.pDynamicStates = dynamicStates;

		VkGraphicsPipelineCreateInfo createInfo;
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
		createInfo.renderPass = mBaseFbo->getRenderPass();
		createInfo.subpass = 0U;
		createInfo.basePipelineHandle = VK_NULL_HANDLE;
		createInfo.basePipelineIndex = -1;

		vkCreateGraphicsPipelines(mDevice, mCache, 1, &createInfo, nullptr, &mPipeline);
	}

}