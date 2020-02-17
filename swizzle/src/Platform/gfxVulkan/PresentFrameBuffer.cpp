#include "PresentFrameBuffer.hpp"

namespace swizzle
{

	PresentFrameBuffer::PresentFrameBuffer(VkDevice device, VkImage image, VkImageView imageView, VkFormat imageFormat, uint32_t width, uint32_t height)
		: mDevice(device)
		, mRenderPass(VK_NULL_HANDLE)
		, mFrameBuffer(VK_NULL_HANDLE)
		, mExternalImage(image)
		, mExternalImageView(imageView)
		, mImageFormat(imageFormat)
		, mClearValue()
		, mWidth(width)
		, mHeight(height)
	{
		createRenderPass();
		createFramebuffer();
	}

	PresentFrameBuffer::~PresentFrameBuffer()
	{
		vkDestroyFramebuffer(mDevice, mFrameBuffer, nullptr);
		vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
	}

	const uint32_t PresentFrameBuffer::getNumAttachments() const
	{
		return 1;
	}

	const FramebufferAttachmentType PresentFrameBuffer::getAttachmentType(uint32_t index) const
	{
		index; // index is unused for presentation
		return FramebufferAttachmentType::eColor;
	}

	void PresentFrameBuffer::setClearColor(uint32_t index, ClearColor color)
	{
		index; // index is unused for presentation
		mClearValue.color.float32[0] = color.r;
		mClearValue.color.float32[1] = color.g;
		mClearValue.color.float32[2] = color.b;
		mClearValue.color.float32[3] = color.a;
	}

	// this function does not do anything for presentation
	void PresentFrameBuffer::setClearDepth(uint32_t index, float depthValue, uint8_t stencil)
	{
		index; // index is unused for presentation
		depthValue; // depthValue is unused for presentation
		stencil; // stencil is unused for presentation
	}

	void PresentFrameBuffer::resize(uint32_t width, uint32_t height)
	{

		mWidth = width;
		mHeight = height;

		vkDestroyFramebuffer(mDevice, mFrameBuffer, nullptr);
		vkDestroyRenderPass(mDevice, mRenderPass, nullptr);

		createRenderPass();
		createFramebuffer();
	}

	VkRenderPass PresentFrameBuffer::getRenderPass() const
	{
		return mRenderPass;
	}

	VkFramebuffer PresentFrameBuffer::getFramebuffer() const
	{
		return mFrameBuffer;
	}

	VkRect2D PresentFrameBuffer::getRenderArea() const
	{
		VkRect2D r;
		r.offset.x = 0;
		r.offset.y = 0;
		r.extent.width = mWidth;
		r.extent.height = mHeight;
		return r;
	}

	uint32_t PresentFrameBuffer::getImageCount() const
	{
		return 1U;
	}

	VkImage PresentFrameBuffer::getImage(uint32_t index) const
	{
		index;
		return mExternalImage;
	}

	VkSampleCountFlagBits PresentFrameBuffer::getMultisampleCount() const
	{
		return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	}

	VkClearValue* PresentFrameBuffer::getImageClearValues() const
	{
		VkClearValue* clr = const_cast<VkClearValue*>(&mClearValue);
		return clr;
	}

	void PresentFrameBuffer::createRenderPass()
	{

		VkAttachmentReference attach;
		attach.attachment = 0U;
		attach.layout = VkImageLayout::VK_IMAGE_LAYOUT_GENERAL;

		VkSubpassDescription subPass;
		subPass.flags = VkSubpassDescriptionFlagBits::VK_SUBPASS_DESCRIPTION_PER_VIEW_ATTRIBUTES_BIT_NVX;
		subPass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
		subPass.inputAttachmentCount = 0;
		subPass.pInputAttachments = VK_NULL_HANDLE;
		subPass.colorAttachmentCount = 1;
		subPass.pColorAttachments = &attach;
		subPass.pResolveAttachments = VK_NULL_HANDLE;
		subPass.pDepthStencilAttachment = VK_NULL_HANDLE;
		subPass.preserveAttachmentCount = 0;
		subPass.pPreserveAttachments = VK_NULL_HANDLE;
		
		subPass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;

		VkAttachmentDescription descr;

		descr.flags = 0;
		descr.format = mImageFormat;
		descr.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		descr.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
		descr.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
		descr.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		descr.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
		descr.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		descr.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkRenderPassCreateInfo createInfo;

		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.pNext = VK_NULL_HANDLE;
		createInfo.flags = 0;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &descr;
		createInfo.subpassCount = 1;
		createInfo.pSubpasses = &subPass;
		createInfo.dependencyCount = 0;
		createInfo.pDependencies = VK_NULL_HANDLE;

		VkResult result = vkCreateRenderPass(mDevice, &createInfo, nullptr, &mRenderPass);
		if (result != VK_SUCCESS)
		{
			printf("vkCreateRenderPass failed\n");
		}
	}

	void PresentFrameBuffer::createFramebuffer()
	{
		mClearValue.color.float32[0] = 0.8F;
		mClearValue.color.float32[1] = 0.6F;
		mClearValue.color.float32[2] = 0.2F;
		mClearValue.color.float32[3] = 1.0F;

		//
		//mClearValue.color.int32[0] = 255;
		//mClearValue.color.int32[1] = 255;
		//mClearValue.color.int32[2] = 255;
		//mClearValue.color.int32[3] = 255;

		//mClearValue.color.uint32[0] = 0xFF;
		//mClearValue.color.uint32[1] = 0xAA;
		//mClearValue.color.uint32[2] = 0x77;
		//mClearValue.color.uint32[3] = 0x33;

		//mClearValue.depthStencil.depth = 0.0F;
		//mClearValue.depthStencil.stencil = 0U;

		VkFramebufferCreateInfo createInfo;

		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.pNext = VK_NULL_HANDLE;
		createInfo.flags = 0;
		createInfo.renderPass = mRenderPass;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &mExternalImageView;
		createInfo.width = mWidth;
		createInfo.height = mHeight;
		createInfo.layers = 1;

		vkCreateFramebuffer(mDevice, &createInfo, nullptr, &mFrameBuffer);
	}

}
