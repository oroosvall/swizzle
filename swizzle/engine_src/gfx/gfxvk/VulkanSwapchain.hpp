#ifndef VULKAN_SWAPCHAIN_HPP
#define VULKAN_SWAPCHAIN_HPP

#include <swizzle/core/Resource.hpp>
#include <swizzle/core/Window.hpp>

#include <swizzle/gfx/Swapchain.hpp>

#include "backend/Vk.hpp"
#include "backend/VulkanObjectContainer.hpp"
#include "VulkanFrameBuffer.hpp"

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
        VulkanSwapchain(core::Resource<core::Window> window, const VulkanObjectContainer& vkObjects);
        virtual ~VulkanSwapchain();

        virtual SwBool isVsyncModeSupported(VSyncTypes sync) const override;
        virtual void setVsync(VSyncTypes sync) override;

        virtual void setClearColor(ClearColor color) override;

        virtual core::Resource<Shader> createShader(const ShaderAttributeList& attribs) override;

        virtual void prepare() override;
        virtual void present() override;

        void resize();

        core::Resource<FrameBuffer> getFrameBuffer() const;
        VkSemaphore getSemaphore() const { return mRenderCompleteSemaphore; }

    private:

        std::vector<VkPresentModeKHR> getAvailablePresentModes();

        void recreateSwapchain();
        void createSwapchain(VkSwapchainKHR oldSwapchain);

        void createSynchronizationObjects();

        void createSwapchainImages();
        void createSwapchainImageViews();

        void createFrameBuffers();

        core::Resource<core::Window> mWindow;
        const VulkanObjectContainer& mVkObjects;

        VkSurfaceKHR mSurface;

        U32 mSurfaceWidth;
        U32 mSurfaceHeight;
        VkFormat mFormat;
        VkPresentModeKHR mSelectedPresentMode;

        VkSwapchainKHR mSwapchain;

        std::unordered_map<VSyncTypes, VkPresentModeKHR> mAvailablePresentModes;
        SwBool mRecreateSwapchain;
        U32 mCurrentImage;

        VkFence mAcquireImageFence;
        VkSemaphore mRenderCompleteSemaphore;

        U32 mSwapchainImageCount;
        std::vector<SwapchainImage> mSwapchainImages;

        std::vector<core::Resource<PresentFrameBuffer>> mFrameBuffers;

    };

}

#endif