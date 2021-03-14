#ifndef VULKAN_PRESENT_FBO_HPP
#define VULKAN_PRESENT_FBO_HPP

/* Include files */

#include "backend/Vk.hpp"
#include "backend/VkContainer.hpp"
#include "VulkanTexture.hpp"

#include <swizzle/gfx/FrameBuffer.hpp>

#include <common/Resource.hpp>

#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::gfx
{

    class PresentFBO : public FrameBuffer
    {
    public:
        PresentFBO(const VkContainer vkObjects, VkImage image, VkImageView imageView, VkFormat imageFormat, U32 width,
            U32 height);
        virtual ~PresentFBO();

        // Inherited via FrameBuffer
        virtual U32 getNumColorAttachments() const override;
        virtual SwBool hasDepthAttachment() const override;
        virtual void setColorAttachmentClearColor(U32 attachmentIndex, ClearColor color) override;
        virtual void setDepthAttachmentClearValue(F32 depthValue, U8 stencilValue) override;

        virtual void resize(U32 width, U32 height) override;

        virtual core::Resource<Shader> createShader(const ShaderAttributeList& attributeList) override;

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
        void allocDepthMemory();
        void createDepthImage();

        void createRenderPass();
        void createFramebuffer();

        const VkContainer mVkObjects;
        VkRenderPass mRenderPass;
        VkFramebuffer mFrameBuffer;

        VkImage mExternalImage;
        VkImageView mExternalImageView;
        VkFormat mImageFormat;

        VkClearValue mClearValue;

        U32 mWidth;
        U32 mHeight;

        VkImage mDepthImage;
        VkImageView mDepthImageView;

        VkDeviceMemory mDepthMemory;
        VkClearValue mDepthClearValue;

    };
}

/* Function Declaration */

#endif