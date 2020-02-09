#include "VulkanSwapchain.hpp"
#include "VulkanDevice.hpp"
#include <vector>

namespace swizzle
{

    VulkanSwapchain::VulkanSwapchain(LogicalDevice& logDevice, VkSurfaceKHR surface, VulkanMemoryHelper& memHelper)
        : mDevice(logDevice), mSurface(surface), mMemHelper(memHelper), mSwapchain(VK_NULL_HANDLE),
          mAcquireImageFence(VK_NULL_HANDLE), mSurfaceWidth(0U), mSurfaceHeight(0U),
          mFormat(VkFormat::VK_FORMAT_UNDEFINED), mRecreateSwapchain(false), mSwapchainImages(), mCurrentImage(0U), mSwapchainImageCount(0U)
		, mSelectedPresentMode(VK_PRESENT_MODE_FIFO_KHR)
    {

        createSwapchain(VK_NULL_HANDLE);

		VkFenceCreateInfo fenceInfo;

		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.pNext = VK_NULL_HANDLE;
		fenceInfo.flags = 0;

		vkCreateFence(mDevice.getLogical(), &fenceInfo, nullptr, &mAcquireImageFence);

		createSwapchainImages();
		createSwapchainImageViews();

		createFrameBuffers();

    }

    VulkanSwapchain::~VulkanSwapchain()
    {
		for (SwapchainImage img : mSwapchainImages)
		{
			vkDestroyImageView(mDevice.getLogical(), img.mImageView, nullptr);
		}

		vkDestroyFence(mDevice.getLogical(), mAcquireImageFence, nullptr);

        vkDestroySwapchainKHR(mDevice.getLogical(), mSwapchain, nullptr);
    }

	bool VulkanSwapchain::hasPresentMode(VSyncTypes type)
	{
		bool supported = false;
		auto it = mAvailablePresentModes.find(type);
		if (it != mAvailablePresentModes.end())
		{
			supported = true;
		}
		return supported;
	}

	void VulkanSwapchain::setPresentMode(VSyncTypes type)
	{
		auto it = mAvailablePresentModes.find(type);
		if (it != mAvailablePresentModes.end())
		{
			mSelectedPresentMode = it->second;
			mRecreateSwapchain = true;
		}
	}

	VkImage VulkanSwapchain::getCurrentImage() const
	{
		return mSwapchainImages[mCurrentImage].mImage;
	}

	VkImageView VulkanSwapchain::getCurrentImageView() const
	{
		return mSwapchainImages[mCurrentImage].mImageView;
	}

	Resource<PresentFrameBuffer> VulkanSwapchain::getCurrentFramebuffer() const
	{
		return mFrameBuffer[mCurrentImage];
	}

    void VulkanSwapchain::resize()
    {
		mRecreateSwapchain = true;
    }

    void VulkanSwapchain::prepare() {
		VkResult result = vkAcquireNextImageKHR(mDevice.getLogical(), mSwapchain, UINT64_MAX, VK_NULL_HANDLE, mAcquireImageFence, &mCurrentImage);
		
		if (result >= VK_SUCCESS)
		{
			vkWaitForFences(mDevice.getLogical(), 1, &mAcquireImageFence, VK_TRUE, UINT64_MAX);
			vkResetFences(mDevice.getLogical(), 1, &mAcquireImageFence);
		}
		else
		{
			// dirty resize detection
			printf("%d\n", result);

			mFrameBuffer.clear();
			recreateSwapchain();
		}

		// add result check
		//mSwapchainResultTrigger.check(result);
	}

    void VulkanSwapchain::present() {
	
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &mSwapchain;
		presentInfo.pImageIndices = &mCurrentImage;
		presentInfo.waitSemaphoreCount = 0;
		//presentInfo.pWaitSemaphores = &mVkRenderingFinished;

		VkQueue queue = mDevice.getQueue(0, 0);

		VkResult r = vkQueuePresentKHR(queue, &presentInfo);
		r;
		// add result check
		//mSwapchainResultTrigger.check(r);

		vkDeviceWaitIdle(mDevice.getLogical());

		if (mRecreateSwapchain)
		{
			mRecreateSwapchain = false;
			mFrameBuffer.clear();

			recreateSwapchain();

			//for (uint32_t i = 0; i < mSwapchainImageCount; i++) {
			//	vkDestroyFramebuffer(mVkInstance.mVkDevice, mVkFramebuffers[i], mVkAllocCallback);
			//	vkDestroyImageView(mVkInstance.mVkDevice, mVkImageViews[i], mVkAllocCallback);
			//}

			//createImageView();
			//createFrameBuffer();
		}
	}

	std::vector<VkPresentModeKHR> VulkanSwapchain::getAvailablePresentModes()
	{
		uint32_t presentCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice.getPhysical(), mSurface, &presentCount, nullptr);

		std::vector<VkPresentModeKHR> presentModes;
		presentModes.resize(presentCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice.getPhysical(), mSurface, &presentCount, presentModes.data());

		return presentModes;
	}
	
	void VulkanSwapchain::recreateSwapchain()
	{
		createSwapchain(mSwapchain);
		for (SwapchainImage img : mSwapchainImages)
		{
			vkDestroyImageView(mDevice.getLogical(), img.mImageView, nullptr);
		}
		mSwapchainImages.clear();
		createSwapchainImages();
		createSwapchainImageViews();

		createFrameBuffers();
	}

    void VulkanSwapchain::createSwapchain(VkSwapchainKHR oldSwapchain)
    {
        VkSwapchainKHR newSwapchain = VK_NULL_HANDLE;
        
		std::vector<VkPresentModeKHR> presentModes = getAvailablePresentModes();
		
		for (auto p : presentModes)
		{
			if (p == VK_PRESENT_MODE_FIFO_KHR)
			{
				mAvailablePresentModes[VSyncTypes::eSync_on] = p;
			}
			else if (p == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
			{
				mAvailablePresentModes[VSyncTypes::eSync_adaptive] = p;
			}
			else if (p == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				mAvailablePresentModes[VSyncTypes::eSync_off] = p;
			}
			else if (p == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				auto it = mAvailablePresentModes.find(VSyncTypes::eSync_off);
				if (it == mAvailablePresentModes.end())
				{
					mAvailablePresentModes[VSyncTypes::eSync_off] = p;
				}
			}
		}

		VkSurfaceCapabilitiesKHR surfaceCapabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice.getPhysical(), mSurface, &surfaceCapabilities);

        if (surfaceCapabilities.currentExtent.width < UINT32_MAX)
        {
            mSurfaceWidth = surfaceCapabilities.currentExtent.width;
            mSurfaceHeight = surfaceCapabilities.currentExtent.height;
        }

		VkFormat targetFormat = VK_FORMAT_R8G8B8A8_SRGB;
		VkColorSpaceKHR targetColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

		uint32_t surfaceFormatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice.getPhysical(), mSurface, &surfaceFormatCount, nullptr);
		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		surfaceFormats.resize(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice.getPhysical(), mSurface, &surfaceFormatCount,
			surfaceFormats.data());

		bool found = false;
		uint32_t idx = 0;

		for (uint32_t i = 0; i < surfaceFormatCount; i++)
		{
			if (surfaceFormats[i].colorSpace == targetColorSpace && surfaceFormats[i].format == targetFormat)
			{
				found = true;
				idx = i;
				break;
			}
		}
		VkBool32 supported = FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(mDevice.getPhysical(), 0, mSurface, &supported);

		if (!supported)
		{
			throw;
		}

		// if we does not find one we just use the first provided
		VkColorSpaceKHR colorSpace = surfaceFormats[idx].colorSpace;
		mFormat= surfaceFormats[idx].format;

		uint32_t primaryQueueFamilyIndex = 0;

        VkSwapchainCreateInfoKHR createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.surface = mSurface;
        createInfo.minImageCount = 2;
        createInfo.imageFormat = mFormat;
        createInfo.imageColorSpace = colorSpace;
        createInfo.imageExtent.height = mSurfaceHeight;
        createInfo.imageExtent.width = mSurfaceWidth;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        createInfo.imageSharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 1;
        createInfo.pQueueFamilyIndices = &primaryQueueFamilyIndex;
        createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = mSelectedPresentMode; // check available present modes
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = oldSwapchain;

        vkCreateSwapchainKHR(mDevice.getLogical(), &createInfo, nullptr, &newSwapchain);
        mSwapchain = newSwapchain;

		if (oldSwapchain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(mDevice.getLogical(), oldSwapchain, nullptr);
		}

    }

	void VulkanSwapchain::createSwapchainImages()
	{
		VkResult r = vkGetSwapchainImagesKHR(mDevice.getLogical(), mSwapchain, &mSwapchainImageCount, nullptr);

		mSwapchainImages.resize(mSwapchainImageCount);

		std::vector<VkImage> img(mSwapchainImageCount);

		r = vkGetSwapchainImagesKHR(mDevice.getLogical(), mSwapchain, &mSwapchainImageCount, img.data());

		for (uint32_t i = 0; i < mSwapchainImageCount; ++i)
		{
			mSwapchainImages[i].mImage = img[i];
		}
	}

	void VulkanSwapchain::createSwapchainImageViews()
	{
		for (SwapchainImage& img : mSwapchainImages)
		{
			VkImageViewCreateInfo imViewInfo;
			imViewInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imViewInfo.flags = 0;
			imViewInfo.format = mFormat;
			imViewInfo.image = img.mImage;
			imViewInfo.pNext = nullptr;
			imViewInfo.subresourceRange.aspectMask =
				VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			imViewInfo.subresourceRange.baseArrayLayer = 0;
			imViewInfo.subresourceRange.baseMipLevel = 0;
			imViewInfo.subresourceRange.layerCount = 1;
			imViewInfo.subresourceRange.levelCount = 1;
			imViewInfo.components.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A;
			imViewInfo.components.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
			imViewInfo.components.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
			imViewInfo.components.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
			imViewInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;

			VkResult result = vkCreateImageView(mDevice.getLogical(), &imViewInfo, nullptr, &img.mImageView);
			result;
			//VK_CALL(result == VK_SUCCESS, "Failed to create VkImageView!\n");
		}
	}


	void VulkanSwapchain::createFrameBuffers()
	{
		for (SwapchainImage& img : mSwapchainImages)
		{
			auto f = std::make_shared<PresentFrameBuffer>(mDevice.getLogical(), img.mImage, img.mImageView, mFormat, mSurfaceWidth, mSurfaceHeight);
			mFrameBuffer.emplace_back(f);
			
		}
	}

} // namespace renderer