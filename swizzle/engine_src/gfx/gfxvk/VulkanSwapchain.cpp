#include "VulkanSwapchain.hpp"
#include "Surface.hpp"
#include "VulkanFrameBuffer.hpp"

#include "shader/VulkanShader.hpp"
#include <swizzle/core/Logging.hpp>

namespace swizzle::gfx
{


    VulkanSwapchain::VulkanSwapchain(core::Resource<core::Window> window, const VulkanObjectContainer& vkObjects)
        : mWindow(window)
        , mVkObjects(vkObjects)
        , mSurface(VK_NULL_HANDLE)
        , mSurfaceWidth(0U)
        , mSurfaceHeight(0U)
        , mFormat(VkFormat::VK_FORMAT_UNDEFINED)
        //, mSelectedPresentMode(VK_PRESENT_MODE_FIFO_KHR)
        , mSelectedPresentMode(VK_PRESENT_MODE_IMMEDIATE_KHR)
        , mSwapchain(VK_NULL_HANDLE)
        , mAvailablePresentModes()
        , mRecreateSwapchain(false)
        , mCurrentImage(0U)
        , mAcquireImageFence(VK_NULL_HANDLE)
        , mRenderCompleteSemaphore(VK_NULL_HANDLE)
        , mSwapchainImageCount(0U)
        , mSwapchainImages()
        , mFrameBuffers()
    {
        mSurface = CreateOsSurface(window, mVkObjects.mInstance);

        createSwapchain(VK_NULL_HANDLE);
        createSynchronizationObjects();

        createSwapchainImages();
        createSwapchainImageViews();

        createFrameBuffers();

    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        vkDeviceWaitIdle(mVkObjects.mLogicalDevice);
        mFrameBuffers.clear();

        vkDestroySemaphore(mVkObjects.mLogicalDevice, mRenderCompleteSemaphore, mVkObjects.mDebugAllocCallbacks);
        vkDestroyFence(mVkObjects.mLogicalDevice, mAcquireImageFence, mVkObjects.mDebugAllocCallbacks);

        for (SwapchainImage img : mSwapchainImages)
        {
            vkDestroyImageView(mVkObjects.mLogicalDevice, img.mImageView, mVkObjects.mDebugAllocCallbacks);
        }

        vkDestroySwapchainKHR(mVkObjects.mLogicalDevice, mSwapchain, mVkObjects.mDebugAllocCallbacks);
        vkDestroySurfaceKHR(mVkObjects.mInstance, mSurface, nullptr);

        mAcquireImageFence = VK_NULL_HANDLE;
        mSwapchain = VK_NULL_HANDLE;
        mSurface = VK_NULL_HANDLE;
    }

    SwBool VulkanSwapchain::isVsyncModeSupported(VSyncTypes sync) const
    {
        SwBool supported = false;
        auto it = mAvailablePresentModes.find(sync);
        if (it != mAvailablePresentModes.end())
        {
            supported = true;
        }
        return supported;
    }

    void VulkanSwapchain::setVsync(VSyncTypes sync)
    {
        auto it = mAvailablePresentModes.find(sync);
        if (it != mAvailablePresentModes.end())
        {
            mSelectedPresentMode = it->second;
            mRecreateSwapchain = true;
        }
    }

    void VulkanSwapchain::setClearColor(ClearColor color)
    {
        for (auto fb : mFrameBuffers)
        {
            fb->setColorAttachmentClearColor(0, color);
        }
    }

    core::Resource<Shader> VulkanSwapchain::createShader(const ShaderAttributeList& attribs)
    {
        return mFrameBuffers[0]->createShader(attribs);
    }

    void VulkanSwapchain::prepare()
    {
        VkResult result = vkAcquireNextImageKHR(mVkObjects.mLogicalDevice, mSwapchain, UINT64_MAX, VK_NULL_HANDLE,
            mAcquireImageFence, &mCurrentImage);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            LOG_INFO("Recrating swapchain: %s", vk::VkResultToString(result));
            recreateSwapchain();
            prepare();
        }
        else if (result < VK_SUCCESS)
        {
            LOG_ERROR("Acquire image error: %s", vk::VkResultToString(result));
        }
        else if (result != VK_SUCCESS)
        {
            LOG_INFO("Acquire image: %s", vk::VkResultToString(result));
        }
    }

    void VulkanSwapchain::present()
    {
        vkWaitForFences(mVkObjects.mLogicalDevice, 1U, &mAcquireImageFence, VK_TRUE, UINT64_MAX);
        vkResetFences(mVkObjects.mLogicalDevice, 1U, &mAcquireImageFence);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.swapchainCount = 1U;
        presentInfo.pSwapchains = &mSwapchain;
        presentInfo.pImageIndices = &mCurrentImage;
        presentInfo.waitSemaphoreCount = 1U;
        presentInfo.pWaitSemaphores = &mRenderCompleteSemaphore;

        VkQueue queue = mVkObjects.mQueue;

        (void)vkQueuePresentKHR(queue, &presentInfo);

        if (mRecreateSwapchain)
        {
            mRecreateSwapchain = false;
            recreateSwapchain();
        }
    }

    void VulkanSwapchain::resize()
    {
        mRecreateSwapchain = true;
    }

    core::Resource<FrameBuffer> VulkanSwapchain::getFrameBuffer() const
    {
        vkWaitForFences(mVkObjects.mLogicalDevice, 1U, &mAcquireImageFence, VK_TRUE, UINT64_MAX);
        return mFrameBuffers[mCurrentImage];
    }

    /*
    *  ------ PRIVATE FUNCTIONS ------
    */

    std::vector<VkPresentModeKHR> VulkanSwapchain::getAvailablePresentModes()
    {
        uint32_t presentCount = 0U;
        vkGetPhysicalDeviceSurfacePresentModesKHR(mVkObjects.mPhysicalDevice, mSurface, &presentCount, nullptr);

        std::vector<VkPresentModeKHR> presentModes;
        presentModes.resize(presentCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(mVkObjects.mPhysicalDevice, mSurface, &presentCount, presentModes.data());

        return presentModes;
    }

    void VulkanSwapchain::recreateSwapchain()
    {
        // make sure device is idle before recreating the swapchain
        vkDeviceWaitIdle(mVkObjects.mLogicalDevice);

        mFrameBuffers.clear();

        createSwapchain(mSwapchain);

        for (SwapchainImage img : mSwapchainImages)
        {
            vkDestroyImageView(mVkObjects.mLogicalDevice, img.mImageView, mVkObjects.mDebugAllocCallbacks);
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
                mAvailablePresentModes[VSyncTypes::vSyncOn] = p;
            }
            else if (p == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
            {
                mAvailablePresentModes[VSyncTypes::vSyncAdaptive] = p;
            }
            else if (p == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                mAvailablePresentModes[VSyncTypes::vSyncOff] = p;
            }
            else if (p == VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
                auto it = mAvailablePresentModes.find(VSyncTypes::vSyncOff);
                if (it == mAvailablePresentModes.end())
                {
                    mAvailablePresentModes[VSyncTypes::vSyncOff] = p;
                }
            }
        }

        VkFormat targetFormat = VK_FORMAT_B8G8R8A8_SRGB;
        VkColorSpaceKHR targetColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        uint32_t surfaceFormatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(mVkObjects.mPhysicalDevice, mSurface, &surfaceFormatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        surfaceFormats.resize(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(mVkObjects.mPhysicalDevice, mSurface, &surfaceFormatCount,
            surfaceFormats.data());

        bool found = false;
        uint32_t idx = 0U;

        for (uint32_t i = 0U; i < surfaceFormatCount; i++)
        {
            if (surfaceFormats[i].colorSpace == targetColorSpace && surfaceFormats[i].format == targetFormat)
            {
                found = true;
                idx = i;
                break;
            }
        }
        VkBool32 supported = FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(mVkObjects.mPhysicalDevice, mVkObjects.mQueueFamilyIndex, mSurface, &supported);

        if (!supported)
        {
            throw;
        }

        // if we does not find one we just use the first provided
        VkColorSpaceKHR colorSpace = surfaceFormats[idx].colorSpace;
        mFormat = surfaceFormats[idx].format;

        VkSurfaceFullScreenExclusiveInfoEXT fullscreenExclusice = {};
        fullscreenExclusice.sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT;
        fullscreenExclusice.pNext = VK_NULL_HANDLE;
        fullscreenExclusice.fullScreenExclusive = VkFullScreenExclusiveEXT::VK_FULL_SCREEN_EXCLUSIVE_DEFAULT_EXT;

        VkSwapchainCreateInfoKHR createInfo = {};

        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mVkObjects.mPhysicalDevice, mSurface, &surfaceCapabilities);

        if (surfaceCapabilities.currentExtent.width < UINT32_MAX)
        {
            mSurfaceWidth = surfaceCapabilities.currentExtent.width;
            mSurfaceHeight = surfaceCapabilities.currentExtent.height;
        }

        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.pNext = &fullscreenExclusice;
        createInfo.flags = 0;
        createInfo.surface = mSurface;
        createInfo.minImageCount = 3U;
        createInfo.imageFormat = mFormat;
        createInfo.imageColorSpace = colorSpace;
        createInfo.imageExtent.height = mSurfaceHeight;
        createInfo.imageExtent.width = mSurfaceWidth;
        createInfo.imageArrayLayers = 1U;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        createInfo.imageSharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 1U;
        createInfo.pQueueFamilyIndices = &mVkObjects.mQueueFamilyIndex;
        createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = mSelectedPresentMode; // check available present modes
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = oldSwapchain;

        VkResult result = vkCreateSwapchainKHR(mVkObjects.mLogicalDevice, &createInfo, mVkObjects.mDebugAllocCallbacks, &newSwapchain);

        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Error creating swapchain %s", vk::VkResultToString(result));
        }

        mSwapchain = newSwapchain;

        if (oldSwapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(mVkObjects.mLogicalDevice, oldSwapchain, mVkObjects.mDebugAllocCallbacks);
        }
    }

    void VulkanSwapchain::createSynchronizationObjects()
    {
        VkFenceCreateInfo fenceInfo;

        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = VK_NULL_HANDLE;
        fenceInfo.flags = 0;

        VkResult result = vkCreateFence(mVkObjects.mLogicalDevice, &fenceInfo, mVkObjects.mDebugAllocCallbacks, &mAcquireImageFence);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Error creating swapchain fence %s", vk::VkResultToString(result));
        }

        VkSemaphoreCreateInfo semaphoreInfo;
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = VK_NULL_HANDLE;
        semaphoreInfo.flags = 0U;

        result = vkCreateSemaphore(mVkObjects.mLogicalDevice, &semaphoreInfo, mVkObjects.mDebugAllocCallbacks, &mRenderCompleteSemaphore);
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Error creating swapchain semaphore %s", vk::VkResultToString(result));
        }
    }

    void VulkanSwapchain::createSwapchainImages()
    {
        VkResult r = vkGetSwapchainImagesKHR(mVkObjects.mLogicalDevice, mSwapchain, &mSwapchainImageCount, nullptr);

        mSwapchainImages.resize(mSwapchainImageCount);

        std::vector<VkImage> img(mSwapchainImageCount);

        r = vkGetSwapchainImagesKHR(mVkObjects.mLogicalDevice, mSwapchain, &mSwapchainImageCount, img.data());

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

            VkResult result = vkCreateImageView(mVkObjects.mLogicalDevice, &imViewInfo, mVkObjects.mDebugAllocCallbacks, &img.mImageView);
            if (result != VK_SUCCESS)
            {
                LOG_ERROR("Error creating swapchain image view %s", vk::VkResultToString(result));
            }
        }
    }

    void VulkanSwapchain::createFrameBuffers()
    {
        for (SwapchainImage& img : mSwapchainImages)
        {
            auto f = std::make_shared<PresentFrameBuffer>(mVkObjects, img.mImage, img.mImageView, mFormat,
                mSurfaceWidth, mSurfaceHeight);
            mFrameBuffers.emplace_back(f);
        }
    }

}