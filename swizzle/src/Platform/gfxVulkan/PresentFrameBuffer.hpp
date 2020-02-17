#ifndef PRESENT_FRAME_BUFFER_HPP
#define PRESENT_FRAME_BUFFER_HPP

#include "vk.hpp"

#include <swizzle/gfx/FrameBuffer.hpp>
#include "VulkanFrameBuffer.hpp"

namespace swizzle
{

	class PresentFrameBuffer : public BaseFrameBuffer
	{
	public:

		PresentFrameBuffer(VkDevice device, VkImage image, VkImageView imageView, VkFormat imageFormat, uint32_t width, uint32_t height);
		virtual ~PresentFrameBuffer();

		// Inherited via FrameBuffer
		virtual const uint32_t getNumAttachments() const override;
		virtual const FramebufferAttachmentType getAttachmentType(uint32_t index) const override;
		virtual void setClearColor(uint32_t index, ClearColor color) override;
		virtual void setClearDepth(uint32_t index, float depthValue, uint8_t stencil) override;

		virtual void resize(uint32_t width, uint32_t height) override;

		// BaseFrameBuffer
		virtual VkRenderPass getRenderPass() const override;
		virtual VkFramebuffer getFramebuffer() const override;

		virtual VkRect2D getRenderArea() const override;
		virtual uint32_t getImageCount() const override;
		VkImage getImage(uint32_t index) const override;

		virtual VkSampleCountFlagBits getMultisampleCount() const override;

		virtual VkClearValue* getImageClearValues() const override;

	private:

		void createRenderPass();
		void createFramebuffer();

		VkDevice mDevice;

		VkRenderPass mRenderPass;
		VkFramebuffer mFrameBuffer;

		VkImage mExternalImage;
		VkImageView mExternalImageView;
		VkFormat mImageFormat;

		VkClearValue mClearValue;

		uint32_t mWidth;
		uint32_t mHeight;

	};

}

#endif