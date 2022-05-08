
/* Include files */

#include "VSwapchain.hpp"

#include "../backend/Vk.hpp"

#include "Device.hpp"
#include "Instance.hpp"
#include "../surface/Surface.hpp"

#include <optick/optick.h>

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
    VSwapchain::VSwapchain(common::Resource<Instance> instance, common::Resource<Device> device, common::Resource<swizzle::core::SwWindow> window)
        : mInstance(instance)
        , mDevice(device)
        , mWindow(window)
        , mSurface(VK_NULL_HANDLE)
        , mSwapchain(VK_NULL_HANDLE)
        , mAvailablePresentModes()
        , mSelectedPresentMode()
        , mRecreateSwapchain(false)

        , mSurfaceWidth(0u)
        , mSurfaceHeight(0u)
        , mFormat(VkFormat::VK_FORMAT_UNDEFINED)

        , mSwapchainImageCount(0u)
        , mCurrentFrame(0u)
        , mImageIndex(0u)
        , mSwapchainImages()
        , mFrameBuffers()
        , mRenderingFinishedSemaphore()
        , mImageAvailableSemaphore()
        , mInFlightFences()
        , mImagesInFlight()

        , mFrameCounter(0ull)
    {
        mSurface = CreateOsSurface(window, mInstance->getInstanceHandle());

        createSwapchain(VK_NULL_HANDLE);

        createSwapchainImages();
        createSwapchainImageViews();

        createSynchronizationObjects();

        createFrameBuffers();
    }

    VSwapchain::~VSwapchain()
    {
        vkDeviceWaitIdle(mDevice->getDeviceHandle());

        mFrameBuffers.clear();

        for (SwapchainImage img : mSwapchainImages)
        {
            vkDestroyImageView(mDevice->getDeviceHandle(), img.mImageView,
                               mDevice->getAllocCallbacks());
        }

        destroySynchronizationObjects();

        vkDestroySwapchainKHR(mDevice->getDeviceHandle(), mSwapchain, mDevice->getAllocCallbacks());
        vkDestroySurfaceKHR(mInstance->getInstanceHandle(), mSurface, nullptr);

        mSwapchain = VK_NULL_HANDLE;
        mSurface = VK_NULL_HANDLE;

    }

    SwBool VSwapchain::isVsyncModeSupported(swizzle::gfx::VSyncTypes sync) const
    {
        SwBool supported = false;
        auto it = mAvailablePresentModes.find(sync);
        if (it != mAvailablePresentModes.end())
        {
            supported = true;
        }
        return supported;
    }

    void VSwapchain::setVsync(swizzle::gfx::VSyncTypes sync)
    {
        auto it = mAvailablePresentModes.find(sync);
        if (it != mAvailablePresentModes.end())
        {
            mSelectedPresentMode = it->second;
            mRecreateSwapchain = true;
        }
    }

    void VSwapchain::setClearColor(swizzle::gfx::ClearColor color)
    {
        for (auto& fb : mFrameBuffers)
        {
            fb->setColorAttachmentClearColor(0u, color);
        }
    }

    void VSwapchain::clearFrameCounter()
    {
        mFrameCounter = 0u;
    }

    U64 VSwapchain::getFrameCounter()
    {
        OPTICK_EVENT("VSwapchain::getFrameCounter()");
        return mFrameCounter;
    }

    common::Resource<swizzle::gfx::Shader> VSwapchain::createShader(const swizzle::gfx::ShaderAttributeList& attribs)
    {
        return mFrameBuffers[0]->createShader(attribs);
    }

    void VSwapchain::prepare()
    {
        OPTICK_EVENT("VSwapchain::prepare");
        VkResult result = vkAcquireNextImageKHR(mDevice->getDeviceHandle(), mSwapchain, UINT64_MAX,
                                                mImageAvailableSemaphore[mCurrentFrame], VK_NULL_HANDLE, &mImageIndex);
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

    void VSwapchain::present()
    {
        OPTICK_EVENT("VSwapchain::Present");

        VkSemaphore sems[] = { mRenderingFinishedSemaphore[mCurrentFrame] };

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.swapchainCount = 1U;
        presentInfo.pSwapchains = &mSwapchain;
        presentInfo.pImageIndices = &mImageIndex;
        presentInfo.waitSemaphoreCount = COUNT_OF(sems);
        presentInfo.pWaitSemaphores = sems;
        presentInfo.pResults = VK_NULL_HANDLE;

        VkQueue queue = mDevice->getQueue();

        // VkResult res =
        //OPTICK_GPU_FLIP(mSwapchain);
        (void)vkQueuePresentKHR(queue, &presentInfo);
        // LOG_ERROR("Present result %d\n", res);

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

    void VSwapchain::resize()
    {
        mRecreateSwapchain = true;
    }

    common::Resource<swizzle::gfx::FrameBuffer> VSwapchain::getFrameBuffer() const
    {
        // vkWaitForFences(mVkObjects.mLogicalDevice->getLogical(), 1U, &mAcquireImageFence, VK_TRUE, UINT64_MAX);
        return mFrameBuffers[mImageIndex];
    }

    VkSemaphore VSwapchain::getSemaphoreToSignal() const
    {
        return mRenderingFinishedSemaphore[mImageIndex];
    }

    VkSemaphore VSwapchain::getWaitForSemaphore() const
    {
        return mImageAvailableSemaphore[mImageIndex];
    }

    VkFence VSwapchain::getWaitFence() const
    {
        return mInFlightFences[mCurrentFrame];
    }

    U32 VSwapchain::getCurrentFrame() const
    {
        return mCurrentFrame;
    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */

namespace vk
{
    std::vector<VkPresentModeKHR> VSwapchain::getAvailablePresentModes()
    {
        uint32_t presentCount = 0U;
        vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalHandle(), mSurface, &presentCount,
                                                  nullptr);

        std::vector<VkPresentModeKHR> presentModes;
        presentModes.resize(presentCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalHandle(), mSurface, &presentCount,
                                                  presentModes.data());

        return presentModes;
    }

    void VSwapchain::recreateSwapchain()
    {
        // make sure device is idle before recreating the swapchain
        vkDeviceWaitIdle(mDevice->getDeviceHandle());

        mCurrentFrame = 0u;
        mImageIndex = 0u;
        mFrameBuffers.clear();

        createSwapchain(mSwapchain);

        for (SwapchainImage img : mSwapchainImages)
        {
            vkDestroyImageView(mDevice->getDeviceHandle(), img.mImageView,
                               mDevice->getAllocCallbacks());
        }

        destroySynchronizationObjects();
        mSwapchainImages.clear();

        createSwapchainImages();
        createSwapchainImageViews();

        createSynchronizationObjects();

        createFrameBuffers();
    }

    void VSwapchain::createSwapchain(VkSwapchainKHR oldSwapchain)
    {
        VkSwapchainKHR newSwapchain = VK_NULL_HANDLE;

        std::vector<VkPresentModeKHR> presentModes = getAvailablePresentModes();

        for (auto p : presentModes)
        {
            if (p == VK_PRESENT_MODE_FIFO_KHR)
            {
                mAvailablePresentModes[swizzle::gfx::VSyncTypes::vSyncOn] = p;
            }
            else if (p == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
            {
                mAvailablePresentModes[swizzle::gfx::VSyncTypes::vSyncAdaptive] = p;
            }
            else if (p == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                mAvailablePresentModes[swizzle::gfx::VSyncTypes::vSyncOff] = p;
            }
            else if (p == VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
                auto it = mAvailablePresentModes.find(swizzle::gfx::VSyncTypes::vSyncOff);
                if (it == mAvailablePresentModes.end())
                {
                    mAvailablePresentModes[swizzle::gfx::VSyncTypes::vSyncOff] = p;
                }
            }
        }

        VkFormat targetFormat = VK_FORMAT_B8G8R8A8_SRGB;
        VkColorSpaceKHR targetColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        U32 surfaceFormatCount = 0u;
        vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalHandle(), mSurface, &surfaceFormatCount,
                                             nullptr);
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        surfaceFormats.resize(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalHandle(), mSurface, &surfaceFormatCount,
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
        vkGetPhysicalDeviceSurfaceSupportKHR(mDevice->getPhysicalHandle(), queueIndex, mSurface,
                                             &supported);

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
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->getPhysicalHandle(), mSurface,
                                                  &surfaceCapabilities);

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

        VkResult result = vkCreateSwapchainKHR(mDevice->getDeviceHandle(), &createInfo, mDevice->getAllocCallbacks(), &newSwapchain);

        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Error creating swapchain %s", vk::VkResultToString(result));
        }

        mSwapchain = newSwapchain;

        if (oldSwapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(mDevice->getDeviceHandle(), oldSwapchain, mDevice->getAllocCallbacks());
        }
    }

    void VSwapchain::createSynchronizationObjects()
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

            VkResult result = vkCreateFence(mDevice->getDeviceHandle(), &fenceInfo,
                                            mDevice->getAllocCallbacks(), &mInFlightFences[i]);
            if (result != VK_SUCCESS)
            {
                LOG_ERROR("Error creating swapchain fence %s", vk::VkResultToString(result));
            }

            VkSemaphoreCreateInfo semaphoreInfo = {};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphoreInfo.pNext = VK_NULL_HANDLE;
            semaphoreInfo.flags = 0U;

            result = vkCreateSemaphore(mDevice->getDeviceHandle(), &semaphoreInfo,
                                       mDevice->getAllocCallbacks(), &mRenderingFinishedSemaphore[i]);
            if (result != VK_SUCCESS)
            {
                LOG_ERROR("Error creating swapchain semaphore %s", vk::VkResultToString(result));
            }
            result = vkCreateSemaphore(mDevice->getDeviceHandle(), &semaphoreInfo,
                                       mDevice->getAllocCallbacks(), &mImageAvailableSemaphore[i]);
            if (result != VK_SUCCESS)
            {
                LOG_ERROR("Error creating swapchain semaphore %s", vk::VkResultToString(result));
            }
        }
    }

    void VSwapchain::destroySynchronizationObjects()
    {
        for (U32 i = 0u; i < (U32)mSwapchainImages.size(); ++i)
        {
            printf("Destroying fence 0x%p\n", mInFlightFences[i]);

            vkDestroyFence(mDevice->getDeviceHandle(), mInFlightFences[i], mDevice->getAllocCallbacks());
            vkDestroySemaphore(mDevice->getDeviceHandle(), mRenderingFinishedSemaphore[i],
                               mDevice->getAllocCallbacks());
            vkDestroySemaphore(mDevice->getDeviceHandle(), mImageAvailableSemaphore[i],
                               mDevice->getAllocCallbacks());
        }

        mInFlightFences.clear();
        mImagesInFlight.clear();
        mRenderingFinishedSemaphore.clear();
        mImageAvailableSemaphore.clear();
    }

    void VSwapchain::createSwapchainImages()
    {
        vkGetSwapchainImagesKHR(mDevice->getDeviceHandle(), mSwapchain, &mSwapchainImageCount, nullptr);

        mSwapchainImages.resize(mSwapchainImageCount);

        std::vector<VkImage> img(mSwapchainImageCount);

        vkGetSwapchainImagesKHR(mDevice->getDeviceHandle(), mSwapchain, &mSwapchainImageCount, img.data());

        for (U32 i = 0u; i < mSwapchainImageCount; ++i)
        {
            mSwapchainImages[i].mImage = img[i];
        }
    }

    void VSwapchain::createSwapchainImageViews()
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

            VkResult result = vkCreateImageView(mDevice->getDeviceHandle(), &imViewInfo, mDevice->getAllocCallbacks(), &img.mImageView);
            if (result != VK_SUCCESS)
            {
                LOG_ERROR("Error creating swapchain image view %s", vk::VkResultToString(result));
            }
        }
    }

    void VSwapchain::createFrameBuffers()
    {
        for (SwapchainImage& img : mSwapchainImages)
        {
            auto f = std::make_shared<PresentFBO>(mDevice, img.mImage, img.mImageView, mFormat, mSurfaceWidth,
                                                  mSurfaceHeight);
            mFrameBuffers.emplace_back(f);
        }
    }

}
