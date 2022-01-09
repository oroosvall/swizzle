#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"

#include "VulkanSwapchain.hpp"
#include "surface/Surface.hpp"
#include "VulkanPresentFBO.hpp"

#include "shader/VulkanShader.hpp"
#include <swizzle/core/Logging.hpp>

#include <optick/optick.h>

namespace swizzle::gfx
{


    VulkanSwapchain::VulkanSwapchain(const VkContainer vkObjects, common::Resource<core::SwWindow> window)
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
        , mCurrentFrame(0u)
        , mImageIndex(0u)
        //, mRenderCompleteSemaphore(VK_NULL_HANDLE)
        , mSwapchainImageCount(0u)
        , mSwapchainImages()
        , mFrameBuffers()
        , mRenderingFinishedSemaphore()
        , mImageAvailableSemaphore()
        , mInFlightFences()
        , mImagesInFlight()
        , mFrameCounter(0u)
    {
        mSurface = CreateOsSurface(window, mVkObjects.mVkInstance->getInstance());

        createSwapchain(VK_NULL_HANDLE);

        createSwapchainImages();
        createSwapchainImageViews();

        createSynchronizationObjects();

        createFrameBuffers();

    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        vkDeviceWaitIdle(mVkObjects.mLogicalDevice->getLogical());
        mFrameBuffers.clear();

        //vkDestroySemaphore(mVkObjects.mLogicalDevice->getLogical(), mRenderCompleteSemaphore, mVkObjects.mDebugAllocCallbacks);

        for (SwapchainImage img : mSwapchainImages)
        {
            vkDestroyImageView(mVkObjects.mLogicalDevice->getLogical(), img.mImageView, mVkObjects.mDebugAllocCallbacks);
        }

        destroySunchronizationObjects();

        vkDestroySwapchainKHR(mVkObjects.mLogicalDevice->getLogical(), mSwapchain, mVkObjects.mDebugAllocCallbacks);
        vkDestroySurfaceKHR(mVkObjects.mVkInstance->getInstance(), mSurface, nullptr);

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
        for (auto& fb : mFrameBuffers)
        {
            fb->setColorAttachmentClearColor(0u, color);
        }
    }

    void VulkanSwapchain::clearFrameCounter()
    {
        mFrameCounter = 0u;
    }

    U64 VulkanSwapchain::getFrameCounter()
    {
        return mFrameCounter;
    }

    common::Resource<Shader> VulkanSwapchain::createShader(const ShaderAttributeList& attribs)
    {
        return mFrameBuffers[0]->createShader(attribs);
    }

    void VulkanSwapchain::prepare()
    {
        VkResult result = vkAcquireNextImageKHR(mVkObjects.mLogicalDevice->getLogical(), mSwapchain, UINT64_MAX, mImageAvailableSemaphore[mCurrentFrame],
            VK_NULL_HANDLE, &mImageIndex);

        /*if (mImagesInFlight[mImageIndex] != VK_NULL_HANDLE)
        {
            vkWaitForFences(mVkObjects.mLogicalDevice->getLogical(), 1u, &mImagesInFlight[mImageIndex], VK_TRUE, UINT64_MAX);
        }

        mImagesInFlight[mImageIndex] = mInFlightFences[mCurrentFrame];*/

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

        //OPTICK_GPU_EVENT("Prepare");
    }

    void VulkanSwapchain::present()
    {
        OPTICK_EVENT("Present");
        /*vkWaitForFences(mVkObjects.mLogicalDevice->getLogical(), 1U, &mAcquireImageFence, VK_TRUE, UINT64_MAX);
        vkResetFences(mVkObjects.mLogicalDevice->getLogical(), 1U, &mAcquireImageFence);*/

        VkSemaphore sems[] = { mImageAvailableSemaphore[mCurrentFrame], mRenderingFinishedSemaphore[mCurrentFrame] };

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.swapchainCount = 1U;
        presentInfo.pSwapchains = &mSwapchain;
        presentInfo.pImageIndices = &mImageIndex;
        presentInfo.waitSemaphoreCount = 2U;
        presentInfo.pWaitSemaphores = sems; // &mRenderingFinishedSemaphore[mCurrentFrame];
        presentInfo.pResults = VK_NULL_HANDLE;

        VkQueue queue = mVkObjects.mLogicalDevice->getQueue(0, 0);

        //VkResult res =
        OPTICK_GPU_FLIP(mSwapchain);
        (void)vkQueuePresentKHR(queue, &presentInfo);
        //LOG_ERROR("Present result %d\n", res);

        mCurrentFrame++;
        if (mCurrentFrame == mSwapchainImages.size())
        {
            mCurrentFrame = 0u;
        }

        if (mRecreateSwapchain)
        {
            mRecreateSwapchain = false;
            recreateSwapchain();
        }

        mFrameCounter++;
    }

    void VulkanSwapchain::resize()
    {
        mRecreateSwapchain = true;
    }

    common::Resource<FrameBuffer> VulkanSwapchain::getFrameBuffer() const
    {
        //vkWaitForFences(mVkObjects.mLogicalDevice->getLogical(), 1U, &mAcquireImageFence, VK_TRUE, UINT64_MAX);
        return mFrameBuffers[mImageIndex];
    }

    VkSemaphore VulkanSwapchain::getSemaphoreToSignal() const
    {
        return mRenderingFinishedSemaphore[mImageIndex];
    }

    VkSemaphore VulkanSwapchain::getWaitForSemaphore() const
    {
        return mImageAvailableSemaphore[mImageIndex];
    }

    VkFence VulkanSwapchain::getWaitFence() const
    {
        return mInFlightFences[mCurrentFrame];
    }

    U32 VulkanSwapchain::getCurrentFrame() const
    {
        return mCurrentFrame;
    }

    /*
    *  ------ PRIVATE FUNCTIONS ------
    */

    std::vector<VkPresentModeKHR> VulkanSwapchain::getAvailablePresentModes()
    {
        uint32_t presentCount = 0U;
        vkGetPhysicalDeviceSurfacePresentModesKHR(mVkObjects.mLogicalDevice->getPhysical(), mSurface, &presentCount, nullptr);

        std::vector<VkPresentModeKHR> presentModes;
        presentModes.resize(presentCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(mVkObjects.mLogicalDevice->getPhysical(), mSurface, &presentCount, presentModes.data());

        return presentModes;
    }

    void VulkanSwapchain::recreateSwapchain()
    {
        // make sure device is idle before recreating the swapchain
        vkDeviceWaitIdle(mVkObjects.mLogicalDevice->getLogical());

        mCurrentFrame = 0u;
        mImageIndex = 0u;
        mFrameBuffers.clear();

        createSwapchain(mSwapchain);


        for (SwapchainImage img : mSwapchainImages)
        {
            vkDestroyImageView(mVkObjects.mLogicalDevice->getLogical(), img.mImageView, mVkObjects.mDebugAllocCallbacks);
        }

        destroySunchronizationObjects();
        mSwapchainImages.clear();

        createSwapchainImages();
        createSwapchainImageViews();

        createSynchronizationObjects();

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

        U32 surfaceFormatCount = 0u;
        vkGetPhysicalDeviceSurfaceFormatsKHR(mVkObjects.mLogicalDevice->getPhysical(), mSurface, &surfaceFormatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        surfaceFormats.resize(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(mVkObjects.mLogicalDevice->getPhysical(), mSurface, &surfaceFormatCount,
            surfaceFormats.data());

        bool found = false;
        U32 idx = 0u;

        for (U32 i = 0u; i < surfaceFormatCount; i++)
        {
            if (surfaceFormats[i].colorSpace == targetColorSpace && surfaceFormats[i].format == targetFormat)
            {
                found = true;
                idx = i;
                break;
            }
        }
        VkBool32 supported = VK_FALSE;
        U32 queueIndex = 0u; // TODO: Fix me
        vkGetPhysicalDeviceSurfaceSupportKHR(mVkObjects.mLogicalDevice->getPhysical(), queueIndex, mSurface, &supported);

        if (!supported && !found)
        {
            throw;
        }

        // if we does not find one we just use the first provided
        VkColorSpaceKHR colorSpace = surfaceFormats[idx].colorSpace;
        mFormat = surfaceFormats[idx].format;

        /*VkSurfaceFullScreenExclusiveInfoEXT fullscreenExclusice = {};
        fullscreenExclusice.sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT;
        fullscreenExclusice.pNext = VK_NULL_HANDLE;
        fullscreenExclusice.fullScreenExclusive = VkFullScreenExclusiveEXT::VK_FULL_SCREEN_EXCLUSIVE_DEFAULT_EXT;*/

        VkSwapchainCreateInfoKHR createInfo = {};

        VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mVkObjects.mLogicalDevice->getPhysical(), mSurface, &surfaceCapabilities);

        if (surfaceCapabilities.currentExtent.width < UINT32_MAX)
        {
            mSurfaceWidth = surfaceCapabilities.currentExtent.width;
            mSurfaceHeight = surfaceCapabilities.currentExtent.height;
        }

        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.pNext = VK_NULL_HANDLE; // &fullscreenExclusice;
        createInfo.flags = 0;
        createInfo.surface = mSurface;
        createInfo.minImageCount = 3u;
        createInfo.imageFormat = mFormat;
        createInfo.imageColorSpace = colorSpace;
        createInfo.imageExtent.height = mSurfaceHeight;
        createInfo.imageExtent.width = mSurfaceWidth;
        createInfo.imageArrayLayers = 1u;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        createInfo.imageSharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 1u;
        createInfo.pQueueFamilyIndices = &queueIndex;
        createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = mSelectedPresentMode; // check available present modes
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = oldSwapchain;

        VkResult result = vkCreateSwapchainKHR(mVkObjects.mLogicalDevice->getLogical(), &createInfo, mVkObjects.mDebugAllocCallbacks, &newSwapchain);

        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Error creating swapchain %s", vk::VkResultToString(result));
        }

        mSwapchain = newSwapchain;

        if (oldSwapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(mVkObjects.mLogicalDevice->getLogical(), oldSwapchain, mVkObjects.mDebugAllocCallbacks);
        }
    }

    void VulkanSwapchain::createSynchronizationObjects()
    {
        mInFlightFences.resize(mSwapchainImages.size());
        mImagesInFlight.resize(mSwapchainImages.size());
        mRenderingFinishedSemaphore.resize(mSwapchainImages.size());
        mImageAvailableSemaphore.resize(mSwapchainImages.size());

        VkFenceCreateInfo fenceInfo = {};

        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = VK_NULL_HANDLE;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (U32 i = 0u; i < mSwapchainImages.size(); ++i)
        {

            VkResult result = vkCreateFence(mVkObjects.mLogicalDevice->getLogical(), &fenceInfo, mVkObjects.mDebugAllocCallbacks, &mInFlightFences[i]);
            if (result != VK_SUCCESS)
            {
                LOG_ERROR("Error creating swapchain fence %s", vk::VkResultToString(result));
            }

            VkSemaphoreCreateInfo semaphoreInfo = {};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphoreInfo.pNext = VK_NULL_HANDLE;
            semaphoreInfo.flags = 0U;

            result = vkCreateSemaphore(mVkObjects.mLogicalDevice->getLogical(), &semaphoreInfo, mVkObjects.mDebugAllocCallbacks, &mRenderingFinishedSemaphore[i]);
            if (result != VK_SUCCESS)
            {
                LOG_ERROR("Error creating swapchain semaphore %s", vk::VkResultToString(result));
            }
            result = vkCreateSemaphore(mVkObjects.mLogicalDevice->getLogical(), &semaphoreInfo, mVkObjects.mDebugAllocCallbacks, &mImageAvailableSemaphore[i]);
            if (result != VK_SUCCESS)
            {
                LOG_ERROR("Error creating swapchain semaphore %s", vk::VkResultToString(result));
            }
        }
    }

    void VulkanSwapchain::destroySunchronizationObjects()
    {
        for (U32 i = 0u; i < (U32)mSwapchainImages.size(); ++i)
        {
            printf("Destroying fence 0x%p\n", mInFlightFences[i]);

            vkDestroyFence(mVkObjects.mLogicalDevice->getLogical(), mInFlightFences[i], mVkObjects.mDebugAllocCallbacks);
            vkDestroySemaphore(mVkObjects.mLogicalDevice->getLogical(), mRenderingFinishedSemaphore[i], mVkObjects.mDebugAllocCallbacks);
            vkDestroySemaphore(mVkObjects.mLogicalDevice->getLogical(), mImageAvailableSemaphore[i], mVkObjects.mDebugAllocCallbacks);
        }

        mInFlightFences.clear();
        mImagesInFlight.clear();
        mRenderingFinishedSemaphore.clear();
        mImageAvailableSemaphore.clear();
    }

    void VulkanSwapchain::createSwapchainImages()
    {
        vkGetSwapchainImagesKHR(mVkObjects.mLogicalDevice->getLogical(), mSwapchain, &mSwapchainImageCount, nullptr);

        mSwapchainImages.resize(mSwapchainImageCount);

        std::vector<VkImage> img(mSwapchainImageCount);

        vkGetSwapchainImagesKHR(mVkObjects.mLogicalDevice->getLogical(), mSwapchain, &mSwapchainImageCount, img.data());

        for (U32 i = 0u; i < mSwapchainImageCount; ++i)
        {
            mSwapchainImages[i].mImage = img[i];
        }
    }

    void VulkanSwapchain::createSwapchainImageViews()
    {
        for (SwapchainImage& img : mSwapchainImages)
        {
            VkImageViewCreateInfo imViewInfo = {};
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

            VkResult result = vkCreateImageView(mVkObjects.mLogicalDevice->getLogical(), &imViewInfo, mVkObjects.mDebugAllocCallbacks, &img.mImageView);
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
            auto f = std::make_shared<PresentFBO>(mVkObjects, img.mImage, img.mImageView, mFormat,
                mSurfaceWidth, mSurfaceHeight);
            mFrameBuffers.emplace_back(f);
        }
    }

}