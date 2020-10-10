
#include "VulkanFrameBuffer.hpp"

namespace swizzle
{

    VulkanFrameBuffer::VulkanFrameBuffer(VkDevice device, uint32_t numAttach, eDepthType createDepthStencil, uint32_t width,
                                         uint32_t height, std::vector<Resource<VulkanTexture>> views, VulkanMemoryHelper& memhelper)
        : mCreatedFromImageViews(false), mDevice(device), mRenderPass(VK_NULL_HANDLE), mFramebuffer(VK_NULL_HANDLE), mMemoryHelper(memhelper),
          mNumAttach(numAttach), mWidth(width), mHeight(height),
          mDepthStencil(createDepthStencil)
		// this value can be set with provided image views, however the provider may
                                            // not actually contain a depth/stencil image
          ,
          mTextures(views)
    {
		if (mTextures.size() == 0)
		{
			createTextures();
		}
		else
		{
			mCreatedFromImageViews = true;
		}

        createRenderPass();

        createFramebuffer();
		
    }

    VulkanFrameBuffer::~VulkanFrameBuffer()
    {
        mTextures.clear();
        vkDestroyFramebuffer(mDevice, mFramebuffer, nullptr);
        vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
    }

    //eRenderApi VulkanFrameBuffer::getResourceApiType() const
    //{
    //    return eRenderApi::eRenderer_Vulkan;
    //}

	const uint32_t VulkanFrameBuffer::getNumAttachments() const
	{
		bool hasDepth = (mDepthStencil == eDepthType::eDepth) || mDepthStencil == eDepthType::eDepthStencil;
		return uint32_t(mTextures.size() - (hasDepth ? 1U : 0U));
	}

	const FramebufferAttachmentType VulkanFrameBuffer::getAttachmentType(uint32_t index) const
	{
		index;
		return FramebufferAttachmentType();
	}

	void VulkanFrameBuffer::setClearColor(uint32_t index, ClearColor color)
	{
		mClearValues[index].color.float32[0] = color.r;
		mClearValues[index].color.float32[1] = color.g;
		mClearValues[index].color.float32[2] = color.b;
		mClearValues[index].color.float32[3] = color.a;
	}

	void VulkanFrameBuffer::setClearDepth(uint32_t index, float depthValue, uint8_t stencil)
	{
		mClearValues[index].depthStencil.depth = depthValue;
		mClearValues[index].depthStencil.stencil = stencil;
	}
	
	void VulkanFrameBuffer::resize(uint32_t width, uint32_t height)
	{
		mWidth = width;
		mHeight = height;

		vkDestroyFramebuffer(mDevice, mFramebuffer, nullptr);
		vkDestroyRenderPass(mDevice, mRenderPass, nullptr);

		if (!mCreatedFromImageViews)
		{
			mTextures.clear();
			createTextures();
		}

		createRenderPass();
		createFramebuffer();
	}

    VkRenderPass VulkanFrameBuffer::getRenderPass() const
    {
        return mRenderPass;
    }

    VkFramebuffer VulkanFrameBuffer::getFramebuffer() const
    {
        return mFramebuffer;
    }

	VkRect2D VulkanFrameBuffer::getRenderArea() const
	{
		VkRect2D r;
		r.offset.x = 0;
		r.offset.y = 0;
		r.extent.width = mWidth;
		r.extent.height = mHeight;
		return r;
	}

	uint32_t VulkanFrameBuffer::getImageCount() const
	{
		return static_cast<uint32_t>(mTextures.size());
	}

	VkImage VulkanFrameBuffer::getImage(uint32_t index) const
	{
		return mTextures[index]->getImage();
	}

	VkSampleCountFlagBits VulkanFrameBuffer::getMultisampleCount() const
	{
		return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	}

	VkClearValue* VulkanFrameBuffer::getImageClearValues() const
	{
		VkClearValue* clearValues = const_cast<VkClearValue*>(mClearValues.data());
		return clearValues;
	}

    void VulkanFrameBuffer::resolveAttachments(std::vector<VkAttachmentDescription>& attachDescr,
                                               std::vector<VkAttachmentReference>& attachRefs,
                                               VkAttachmentReference& depthRef)
    {
        uint32_t ctr = 0;

        for (auto it : mTextures)
        {
            const bool isDepth = (it->getImageType() == eTextureResourceType::eTextureResType_RT_depth) || (it->getImageType() == eTextureResourceType::eTextureResType_RT_depthStencil);
            if (isDepth)
            {
                depthRef.attachment = ctr;
                depthRef.layout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
            else
            {
                VkAttachmentReference attach;
                attach.attachment = ctr;
                attach.layout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachRefs.emplace_back(attach);
            }

            VkAttachmentDescription descr;

            descr.flags = 0;
            descr.format = it->getImageViewFormat();
            descr.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
            descr.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
            descr.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
            descr.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
            descr.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;

            if (isDepth)
            {
                descr.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                descr.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				//descr.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
				descr.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				descr.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
            }
            else
            {
                descr.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                descr.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }

            attachDescr.emplace_back(descr);
            ctr++;
        }
    }

    void VulkanFrameBuffer::createRenderPass()
    {
        std::vector<VkAttachmentDescription> attachDescr;
        std::vector<VkAttachmentReference> attachRefs;
        VkAttachmentReference depthRef;

        resolveAttachments(attachDescr, attachRefs, depthRef);

        VkSubpassDescription subPass;
        subPass.flags = 0;
        subPass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
        subPass.inputAttachmentCount = 0;
        subPass.pInputAttachments = VK_NULL_HANDLE;
        subPass.colorAttachmentCount = static_cast<uint32_t>(attachRefs.size());
        subPass.pColorAttachments = attachRefs.data();
        subPass.pResolveAttachments = VK_NULL_HANDLE;
		bool hasDepth = (mDepthStencil == eDepthType::eDepth) || mDepthStencil == eDepthType::eDepthStencil;
        if (hasDepth)
        {
            subPass.pDepthStencilAttachment = &depthRef;
        }
        else
        {
            subPass.pDepthStencilAttachment = VK_NULL_HANDLE;
        }
        subPass.preserveAttachmentCount = 0;
        subPass.pPreserveAttachments = VK_NULL_HANDLE;

		/*VkSubpassDependency dep;
		dep.*/

        VkRenderPassCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0;
        createInfo.attachmentCount = mNumAttach;
        createInfo.pAttachments = attachDescr.data();
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subPass;
        createInfo.dependencyCount = 0;
        createInfo.pDependencies =VK_NULL_HANDLE;

        vkCreateRenderPass(mDevice, &createInfo, nullptr, &mRenderPass);
    }

    void VulkanFrameBuffer::createTextures()
    {
        for (uint32_t i = 0; i < mNumAttach; i++)
        {
            eTextureResourceType resType = eTextureResourceType::eTextureResType_RT_color;
			VkFormat fmt = VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
			bool hasDepth = (mDepthStencil == eDepthType::eDepth) || mDepthStencil == eDepthType::eDepthStencil;
            if (hasDepth && i == 0)
            {
				if (mDepthStencil == eDepthType::eDepth)
				{
					resType = eTextureResourceType::eTextureResType_RT_depth;
					fmt = VkFormat::VK_FORMAT_D32_SFLOAT;
				}
				else if (mDepthStencil == eDepthType::eDepthStencil)
				{
					resType = eTextureResourceType::eTextureResType_RT_depthStencil;
					fmt = VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
				}
            }

			TextureInfo textInfo = {
				mDevice,
				mMemoryHelper,
				mWidth,
				mHeight,
				resType,
				fmt,
				false,
				VK_NULL_HANDLE,
				VK_NULL_HANDLE
			};

            Resource<VulkanTexture> newTexture = std::make_shared<VulkanTexture>(textInfo);
            mTextures.push_back(newTexture);
        }

		/*if (mDepthStencil)
		{
		eTextureResourceType resType = eTextureResourceType::eTextureResType_RT_color;
		VkFormat fmt = VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
			resType = eTextureResourceType::eTextureResType_RT_depth;
			fmt = VkFormat::VK_FORMAT_D32_SFLOAT;

		TextureInfo textInfo = {
			mDevice,
			mMemoryHelper,
			mWidth,
			mHeight,
			resType,
			fmt,
			false,
			VK_NULL_HANDLE,
			VK_NULL_HANDLE
		};

		Resource<VulkanTexture> newTexture = std::make_shared<VulkanTexture>(textInfo);
		mTextures.push_back(newTexture);
		}*/

    }

	VkClearValue createEmptyClearValue()
	{
		VkClearValue clrValue;

		clrValue.color.float32[0] = 0.0F;
		clrValue.color.float32[1] = 0.0F;
		clrValue.color.float32[2] = 0.0F;
		clrValue.color.float32[3] = 0.0F;

		clrValue.color.int32[0] = 0;
		clrValue.color.int32[1] = 0;
		clrValue.color.int32[2] = 0;
		clrValue.color.int32[3] = 0;

		clrValue.color.uint32[0] = 0U;
		clrValue.color.uint32[1] = 0U;
		clrValue.color.uint32[2] = 0U;
		clrValue.color.uint32[3] = 0U;

		clrValue.depthStencil.depth = 0.0F;
		clrValue.depthStencil.stencil = 0U;

		return clrValue;
	}

    void VulkanFrameBuffer::createFramebuffer()
    {
        std::vector<VkImageView> views;

		mClearValues.reserve(mTextures.size());

        for (auto it : mTextures)
        {
            views.push_back(it->getImageView());

			VkClearValue clrValue = createEmptyClearValue();

			mClearValues.emplace_back(clrValue);
        }

        VkFramebufferCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = VK_NULL_HANDLE;
        createInfo.flags = 0;
        createInfo.renderPass = mRenderPass;
        createInfo.attachmentCount = mNumAttach;
        createInfo.pAttachments = views.data();
        createInfo.width = mWidth;
        createInfo.height = mHeight;
        createInfo.layers = 1;

        vkCreateFramebuffer(mDevice, &createInfo, nullptr, &mFramebuffer);
    }

} // namespace renderer
