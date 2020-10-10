//#include "VulkanPipelines.hpp"
//
//
//namespace swizzle
//{
//	VulkanPipeline::VulkanPipeline(VkDevice device, ePipelineType pipelineType)
//		: mDevice(device)
//		, mPipelineType(pipelineType)
//		, mPipelineCache(VK_NULL_HANDLE)
//		, mPipeline(VK_NULL_HANDLE)
//	{
//		switch (pipelineType)
//		{
//		case swizzle::ePipelineType::GraphicsPipeline:
//			createGraphicsPipeline();
//			break;
//		case swizzle::ePipelineType::ComputePipeline:
//			createComputePipeline();
//			break;
//		case swizzle::ePipelineType::RaytracingPipeline:
//			createRaytracingPipeline();
//			break;
//		default:
//			break;
//		}
//
//	}
//
//	VulkanPipeline::~VulkanPipeline()
//	{
//	}
//
//	ePipelineType VulkanPipeline::getPipelineType() const
//	{
//		return ePipelineType();
//	}
//
//	void VulkanPipeline::createGraphicsPipeline()
//	{
//		VkGraphicsPipelineCreateInfo pipelineInfo;
//
//		VkPipelineShaderStageCreateInfo shaderStage;
//		VkShaderModule shaderModule = VK_NULL_HANDLE;
//		VkSpecializationInfo specialInfo;
//
//		specialInfo.dataSize = 0;
//		specialInfo.mapEntryCount = 0;
//		specialInfo.pData = 0;
//		specialInfo.pMapEntries = 0;
//
//		shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//		shaderStage.pNext = VK_NULL_HANDLE;
//		shaderStage.flags = 0;
//		shaderStage.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
//		shaderStage.module = shaderModule;
//		shaderStage.pName = "...";
//		shaderStage.pSpecializationInfo = &specialInfo;
//
//		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//		pipelineInfo.pNext = VK_NULL_HANDLE;
//		pipelineInfo.flags = 0;
//		pipelineInfo.stageCount = 1;
//		pipelineInfo.pStages = &shaderStage;
//		pipelineInfo.pVertexInputState; //
//		pipelineInfo.pInputAssemblyState; //
//		pipelineInfo.pTessellationState; //
//		pipelineInfo.pViewportState; //
//		pipelineInfo.pRasterizationState; //
//		pipelineInfo.pMultisampleState; //
//		pipelineInfo.pDepthStencilState; //
//		pipelineInfo.pColorBlendState; //
//		pipelineInfo.pDynamicState; //
//		pipelineInfo.layout; //
//		pipelineInfo.renderPass; //
//		pipelineInfo.subpass; //
//		pipelineInfo.basePipelineHandle; //
//		pipelineInfo.basePipelineIndex; //
//
//		vkCreateGraphicsPipelines(mDevice, mPipelineCache, 1, &pipelineInfo, nullptr, &mPipeline);
//
//	}
//
//	void VulkanPipeline::createComputePipeline()
//	{
//	}
//
//	void VulkanPipeline::createRaytracingPipeline()
//	{
//	}
//}