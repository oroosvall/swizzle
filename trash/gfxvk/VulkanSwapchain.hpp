#ifndef VULKAN_SWAPCHAIN_HPP
#define VULKAN_SWAPCHAIN_HPP

#include <common/Common.hpp>
#include <swizzle/core/Window.hpp>

#include <swizzle/gfx/Swapchain.hpp>

#include "backend/Vk.hpp"
#include "backend/VkContainer.hpp"
#include "VulkanPresentFBO.hpp"

#include <vector>
#include <unordered_map>

namespace swizzle::gfx
{

    struct SwapchainImage
    {
        VkImage mImage;
        VkImageView mImageView;
    };

    class VulkanSwapchain : public Swapchain
    {
    public:
        VulkanSwapchain(const VkContainer vkObjects, common::Resource<core::SwWindow> window);
        virtual ~VulkanSwapchain();

        virtual SwBool isVsyncModeSupported(VSyncTypes sync) const override;
        virtual void setVsync(VSyncTypes sync) override;

        virtual void setClearColor(ClearColor color) override;

        virtual void clearFrameCounter() override;
        virtual U64 getFrameCounter() override;

        virtual common::Resource<Shader> createShader(const ShaderAttributeList& attribs) override;

        virtual void prepare() override;
        virtual void present() override;

        void resize();

        common::Resource<FrameBuffer> getFrameBuffer() const;
        VkSemaphore getSemaphoreToSignal() const;
        VkSemaphore getWaitForSemaphore() const;
        VkFence getWaitFence() const;
        U32 getCurrentFrame() const;

    private:

        std::vector<VkPresentModeKHR> getAvailablePresentModes();

        void recreateSwapchain();
        void createSwapchain(VkSwapchainKHR oldSwapchain);

        void createSynchronizationObjects();
        void destroySunchronizationObjects();

        void createSwapchainImages();
        void createSwapchainImageViews();

        void createFrameBuffers();

        common::Resource<core::SwWindow> mWindow;
        const VkContainer mVkObjects;

        VkSurfaceKHR mSurface;

        U32 mSurfaceWidth;
        U32 mSurfaceHeight;
        VkFormat mFormat;
        VkPresentModeKHR mSelectedPresentMode;

        VkSwapchainKHR mSwapchain;

        std::unordered_map<VSyncTypes, VkPresentModeKHR> mAvailablePresentModes;
        SwBool mRecreateSwapchain;
        U32 mCurrentFrame;
        U32 mImageIndex;

        //VkSemaphore mRenderCompleteSemaphore;

        U32 mSwapchainImageCount;
        std::vector<SwapchainImage> mSwapchainImages;

        std::vector<common::Resource<PresentFBO>> mFrameBuffers;
        std::vector<VkSemaphore> mRenderingFinishedSemaphore;
        std::vector<VkSemaphore> mImageAvailableSemaphore;

        std::vector<VkFence> mInFlightFences;
        std::vector<VkFence> mImagesInFlight;

        U64 mFrameCounter;

    };

}

#endif