#ifndef VULKAN_SWAPCHAIN_HPP
#define VULKAN_SWAPCHAIN_HPP

#include "vk.hpp"
#include <swizzle/Renderer.hpp>

#include "PresentFrameBuffer.hpp"
#include "VulkanTexture.hpp"

#include <vector>
#include <unordered_map>

namespace swizzle
{

	class LogicalDevice;

	struct SwapchainImage
	{
		VkImage mImage;
		VkImageView mImageView;
	};

    class VulkanSwapchain
    {

    public:
        VulkanSwapchain(LogicalDevice& logDevice, VkSurfaceKHR surface, VulkanMemoryHelper& memHelper);
        ~VulkanSwapchain();

		bool hasPresentMode(VSyncTypes type);
		void setPresentMode(VSyncTypes type);

		VkImage getCurrentImage() const;
		VkImageView getCurrentImageView() const;

		Resource<PresentFrameBuffer> getCurrentFramebuffer() const;

		void resize();

		void prepare();
		void present();

    private:

		std::vector<VkPresentModeKHR> getAvailablePresentModes();
		
		void recreateSwapchain();
        void createSwapchain(VkSwapchainKHR oldSwapchain);

		void createSwapchainImages();
		void createSwapchainImageViews();

		void createFrameBuffers();

		LogicalDevice& mDevice;
		VulkanMemoryHelper& mMemHelper;
		VkSurfaceKHR mSurface;

        VkSwapchainKHR mSwapchain;

		VkFence mAcquireImageFence;

		uint32_t mSurfaceWidth;
		uint32_t mSurfaceHeight;
		VkFormat mFormat;

		bool mRecreateSwapchain;
		std::vector<SwapchainImage> mSwapchainImages;

		std::vector<Resource<PresentFrameBuffer>> mFrameBuffer;
		uint32_t mCurrentImage;
		uint32_t mSwapchainImageCount;
		VkPresentModeKHR mSelectedPresentMode;

		std::unordered_map<VSyncTypes, VkPresentModeKHR> mAvailablePresentModes;

    };

} // namespace renderer

#endif