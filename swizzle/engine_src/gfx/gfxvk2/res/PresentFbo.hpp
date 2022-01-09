#ifndef PRESENT_FBO_HPP
#define PRESENT_FBO_HPP

/* Include files */

#include <swizzle/gfx/FrameBuffer.hpp>

#include "_fwDecl.hpp"
#include "../backend/Vk.hpp"

#include "VkResource.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{
    class PresentFBO : public swizzle::gfx::FrameBuffer
    {
    public:
        PresentFBO(common::Resource<Device> device, VkImage image, VkImageView imageView, VkFormat imageFormat,
                   U32 width, U32 height);
        virtual ~PresentFBO();

        // Inherited via FrameBuffer
        virtual U32 getNumColorAttachments() const override;
        virtual SwBool hasDepthAttachment() const override;
        virtual void setColorAttachmentClearColor(U32 attachmentIndex, swizzle::gfx::ClearColor color) override;
        virtual void setDepthAttachmentClearValue(F32 depthValue, U8 stencilValue) override;

        virtual void resize(U32 width, U32 height) override;

        virtual common::Resource<swizzle::gfx::Shader>
        createShader(const swizzle::gfx::ShaderAttributeList& attributeList) override;

        VkRenderPass getRenderPass() const;
        VkSampleCountFlagBits getMultisampleCount() const;

        // BaseFrameBuffer
        // virtual VkRenderPass getRenderPass() const override;
        // virtual VkFramebuffer getFramebuffer() const override;

        // virtual VkRect2D getRenderArea() const override;
        // virtual uint32_t getImageCount() const override;
        // VkImage getImage(uint32_t index) const override;

        // virtual VkSampleCountFlagBits getMultisampleCount() const override;

        // virtual VkClearValue* getImageClearValues() const override;

        // private:
        //void allocDepthMemory();
        void createDepthImage();

        void createRenderPass();
        void createFramebuffer();

        common::Resource<Device> mDevice;
        VkRenderPass mRenderPass;
        VkFramebuffer mFrameBuffer;

        VkImage mExternalImage;
        VkImageView mExternalImageView;
        VkFormat mImageFormat;

        VkClearValue mClearValue;

        U32 mWidth;
        U32 mHeight;

        //VkImage mDepthImage;
        common::Resource<VkResource<VkImage>> mDepthImage;
        VkImageView mDepthImageView;

        common::Resource<DeviceMemory> mDepthMemory;
        VkClearValue mDepthClearValue;
    };

} // namespace vk

/* Function Declaration */

#endif
