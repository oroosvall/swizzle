#ifndef V_FRAME_BUFFER_HPP
#define V_FRAME_BUFFER_HPP

/* Include files */

#include "BaseFrameBuffer.hpp"

#include "_fwDecl.hpp"
#include "../backend/Vk.hpp"

#include "VkResource.hpp"
#include "TextureBase.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace vk
{
    struct VFrameBufferImage
    {
        common::Resource<VkResource<VkImage>> mImage;
        VkImageView mImageView;

        common::Resource<DeviceMemory> mImageMemory;
        VkClearValue mClearValue;
    };
}

/* Class Declaration */

namespace vk
{
    class DummyTexture : public TextureBase
    {
    public:
        DummyTexture(common::Resource<VkResource<VkImage>> image, VkImageView view)
            : mImage(image)
            , mImageView(view)
        { }
        common::Resource<VkResource<VkImage>> mImage;
        VkImageView mImageView;

        virtual SwBool isUploaded() const
        {
            return false;
        }
        virtual void uploadImage(VkCommandBuffer) {}
        virtual common::Resource<VkResource<VkImage>> getImg()
        {
            return mImage;
        }
        virtual VkImageView getView()
        {
            return mImageView;
        }
        virtual void setData(U32, U32, U32, const U8*) { }
        virtual void getTextureSize(U32&, U32&) { }

        virtual void upload() { }
    };

    class VFrameBuffer : public BaseFrameBuffer
    {
    public:

        VFrameBuffer(common::Resource<Device> device, const swizzle::gfx::FrameBufferCreateInfo& createInfo);

        virtual ~VFrameBuffer();

        virtual U32 getNumColorAttachments() const override;
        virtual SwBool hasDepthAttachment() const override;

        virtual void setColorAttachmentClearColor(U32 attachmentIndex, swizzle::gfx::ClearColor color) override;
        virtual void setDepthAttachmentClearValue(F32 depthValue, U8 stencilValue) override;

        virtual common::Resource<swizzle::gfx::Texture> getColorAttachment(U32 index) override;

        virtual void resize(U32 width, U32 height) override;

        virtual VkRenderPass getRenderPass() const override;
        virtual VkSampleCountFlagBits getMultisampleCount() const override;
        virtual VkFramebuffer getFrameBuffer() const override;

        virtual void getSize(U32& width, U32& height) const override;
        virtual VkClearValue getColorClearValue(U32 index) const override;
        virtual VkClearValue getDepthClearValue() const override;

    private:

        void destroyResources();

        void createColorImages();
        void createDepthImage();

        void createRenderPass();
        void createFramebuffer();

        common::Resource<Device> mDevice;
        const swizzle::gfx::FrameBufferCreateInfo mCreateInfo;

        VkRenderPass mRenderPass;
        VkFramebuffer mFrameBuffer;

        U32 mWidth;
        U32 mHeight;

        SwBool mHasDepthBuffer;
        VFrameBufferImage mDepthImage;

        VkFormat mImageFormat;
        std::vector<VFrameBufferImage> mImages;

        common::Resource<swizzle::gfx::Material> mMaterial;
    };
}

/* Function Declaration */

namespace vk
{
    common::Resource<BaseFrameBuffer> GetFrameBufferAsBaseFrameBuffer(common::Resource<swizzle::gfx::FrameBuffer> buffer);
}

#endif