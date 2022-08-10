#ifndef V_SWAPCHAIN_HPP
#define V_SWAPCHAIN_HPP

/* Include files */

#include <swizzle/gfx/Swapchain.hpp>
#include <swizzle/core/Window.hpp>

#include "_fwDecl.hpp"

#include <vector>
#include <unordered_map>

#include "../backend/Vk.hpp"

#include "PresentFbo.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace vk
{
    struct SwapchainImage
    {
        VkImage mImage;
        VkImageView mImageView;
    };
}

/* Class Declaration */

namespace vk
{
    class VSwapchain : public swizzle::gfx::Swapchain
    {
    public:
        VSwapchain(common::Resource<Instance> instance, common::Resource<Device> device, common::Resource<swizzle::core::SwWindow> window);
        virtual ~VSwapchain();

        virtual SwBool isVsyncModeSupported(swizzle::gfx::VSyncTypes sync) const override;
        virtual void setVsync(swizzle::gfx::VSyncTypes sync) override;

        virtual void setClearColor(swizzle::gfx::ClearColor color) override;

        virtual void clearFrameCounter() override;
        virtual U64 getFrameCounter() override;

        virtual void prepare() override;
        virtual void present() override;

        virtual void resize();

        common::Resource<swizzle::gfx::FrameBuffer> getFrameBuffer() const;
        VkSemaphore getSemaphoreToSignal() const;
        VkSemaphore getWaitForSemaphore() const;
        VkFence getWaitFence() const;
        U32 getCurrentFrame() const;

    private:

        std::vector<VkPresentModeKHR> getAvailablePresentModes();

        void recreateSwapchain();
        void createSwapchain(VkSwapchainKHR oldSwapchain);

        void createSynchronizationObjects();
        void destroySynchronizationObjects();

        void createSwapchainImages();
        void createSwapchainImageViews();

        void createFrameBuffers();

        common::Resource<Instance> mInstance;
        common::Resource<Device> mDevice;
        common::Resource<swizzle::core::SwWindow> mWindow;
        VkSurfaceKHR mSurface;
        VkSwapchainKHR mSwapchain;

        std::unordered_map<swizzle::gfx::VSyncTypes, VkPresentModeKHR> mAvailablePresentModes;
        VkPresentModeKHR mSelectedPresentMode;
        SwBool mRecreateSwapchain;

        U32 mSurfaceWidth;
        U32 mSurfaceHeight;
        VkFormat mFormat;

        U32 mSwapchainImageCount;
        U32 mCurrentFrame;
        U32 mImageIndex;

        std::vector<SwapchainImage> mSwapchainImages;
        std::vector<common::Resource<PresentFBO>> mFrameBuffers;

        std::vector<VkSemaphore> mRenderingFinishedSemaphore;
        std::vector<VkSemaphore> mImageAvailableSemaphore;

        std::vector<VkFence> mInFlightFences;
        std::vector<VkFence> mImagesInFlight;


        U64 mFrameCounter;

    };
}

/* Function Declaration */

#endif
