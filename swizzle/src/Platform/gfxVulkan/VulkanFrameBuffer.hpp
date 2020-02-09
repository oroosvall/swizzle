#ifndef VULKAN_FRAME_BUFFER_HPP
#define VULKAN_FRAME_BUFFER_HPP

#include "vk.hpp"

#include <swizzle/gfx/FrameBuffer.hpp>
#include <swizzle/Resource.hpp>

#include "VulkanTexture.hpp"
#include "VulkanMemoryHelper.hpp"


#include <vector>

namespace swizzle
{

	class BaseFrameBuffer : public FrameBuffer
	{
	public:
		
		virtual VkRenderPass getRenderPass() const = 0;
		virtual VkFramebuffer getFramebuffer() const = 0;

		virtual VkRect2D getRenderArea() const = 0;
		virtual uint32_t getImageCount() const = 0;
		virtual VkImage getImage(uint32_t index) const = 0;

		virtual VkClearValue* getImageClearValues() const = 0;
	};

    class VulkanFrameBuffer : public BaseFrameBuffer
    {
    public:
        VulkanFrameBuffer(VkDevice device, uint32_t numAttach, DepthType createDepthStencil, uint32_t width, uint32_t height,
                          std::vector<Resource<VulkanTexture>> views, VulkanMemoryHelper& memoryHelper);
        virtual ~VulkanFrameBuffer();

        //virtual eRenderApi getResourceApiType() const override;

		// FrameBuffer
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
		virtual VkImage getImage(uint32_t index) const override;

		virtual VkClearValue* getImageClearValues() const override;

    private:
        void createRenderPass();
        void createTextures();
        void createFramebuffer();

        void resolveAttachments(std::vector<VkAttachmentDescription>& attachDescr,
                                std::vector<VkAttachmentReference>& attachRefs, VkAttachmentReference& depthRef);

        bool mCreatedFromImageViews;
        VkDevice mDevice;
        VkRenderPass mRenderPass;
        VkFramebuffer mFramebuffer;
		VulkanMemoryHelper& mMemoryHelper;

        uint32_t mNumAttach;
        uint32_t mWidth;
        uint32_t mHeight;

		DepthType mDepthStencil;
        std::vector<Resource<VulkanTexture>> mTextures;
		std::vector<VkClearValue> mClearValues;
		bool mDest;
    };

} // namespace renderer

#endif