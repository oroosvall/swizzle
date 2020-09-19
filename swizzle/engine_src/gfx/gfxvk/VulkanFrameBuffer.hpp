#ifndef VULKAN_FRAME_BUFFER_HPP
#define VULKAN_FRAME_BUFFER_HPP

#include "vk.hpp"
#include "VulkanObjectContainer.hpp"
#include <swizzle/gfx/FrameBuffer.hpp>


namespace swizzle::gfx
{

    class PresentFrameBuffer : public FrameBuffer
    {
    public:
        PresentFrameBuffer(const VulkanObjectContainer& vkObjects, VkImage image, VkImageView imageView, VkFormat imageFormat, SwU32 width,
            SwU32 height);
        virtual ~PresentFrameBuffer();

        // Inherited via FrameBuffer
        virtual SwU32 getNumColorAttachments() const override;
        virtual SwBool hasDepthAttachment() const override;
        virtual void setColorAttachmentClearColor(SwU32 attachmentIndex, ClearColor color) override;
        virtual void setDepthAttachmentClearValue(SwFloat depthValue, SwU8 stencilValue) override;

        virtual void resize(SwU32 width, SwU32 height) override;

        virtual core::Resource<Shader> createShader(ShaderAttributeList attributeList) override;

        VkRenderPass getRenderPass() const;
        VkSampleCountFlagBits getMultisampleCount() const;

        // BaseFrameBuffer
        //virtual VkRenderPass getRenderPass() const override;
        //virtual VkFramebuffer getFramebuffer() const override;

        //virtual VkRect2D getRenderArea() const override;
        //virtual uint32_t getImageCount() const override;
        //VkImage getImage(uint32_t index) const override;

        //virtual VkSampleCountFlagBits getMultisampleCount() const override;

        //virtual VkClearValue* getImageClearValues() const override;

    //private:
        void createRenderPass();
        void createFramebuffer();

        const VulkanObjectContainer& mVkObjects;
        VkRenderPass mRenderPass;
        VkFramebuffer mFrameBuffer;

        VkImage mExternalImage;
        VkImageView mExternalImageView;
        VkFormat mImageFormat;

        VkClearValue mClearValue;

        SwU32 mWidth;
        SwU32 mHeight;
    };

    /*class VulkanFrameBuffer : public FrameBuffer
    {
    public:

        VulkanFrameBuffer(VulkanObjectContainer& vkObjects);
        virtual ~VulkanFrameBuffer();

        virtual SwU32 getNumColorAttachments() const override;
        virtual SwBool hasDepthAttachment() const override;

        virtual void setColorAttachmentClearColor(SwU32 attachmentIndex, ClearColor color) override;
        virtual void setDepthAttachmentClearValue(SwFloat depthValue, SwU8 stencilValue) override;

        virtual void resize(SwU32 width, SwU32 height) override;

    private:
        
        void createRenderPass();
        void createFrameBuffer();

        VulkanObjectContainer& mVkObjects;

        VkRenderPass mRenderPass;
        VkFramebuffer mFrameBuffer;


    };*/

}

#endif