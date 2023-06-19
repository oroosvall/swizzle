#ifndef V_FRAME_BUFFER_HPP
#define V_FRAME_BUFFER_HPP

/* Include files */

#include "BaseFrameBuffer.hpp"

#include "../backend/Vk.hpp"
#include "_fwDecl.hpp"

#include "TextureBase.hpp"
#include "VkResource.hpp"

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
        VkFormat mFormat;

        common::Resource<DeviceMemory> mImageMemory;
        VkClearValue mClearValue;
    };
} // namespace vk

/* Class Declaration */

namespace vk
{
    class DummyTexture : public TextureBase
    {
    public:
        DummyTexture(common::Resource<VkResource<VkImage>> image, VkImageView view)
            : mImage(image)
            , mImageView(view)
            , mDepth(false)
        {
        }
        common::Resource<VkResource<VkImage>> mImage;
        VkImageView mImageView;
        SwBool mDepth;

        virtual SwBool isUploaded() const override
        {
            return false;
        }
        virtual void uploadImage(VkCommandBuffer) override {}
        virtual common::Resource<VkResource<VkImage>> getImg() override
        {
            return mImage;
        }
        virtual VkImageView getView() override
        {
            return mImageView;
        }
        virtual bool isDepth() override
        {
            return mDepth;
        }
        virtual void setData(U32, U32, U32, const U8*) override {}
        virtual swizzle::gfx::TextureDimensions getSize() const override
        {
            return {1u, 1u, 1u};
        }

        virtual void upload() override {}

        virtual void transferImageToCompute(VkCommandBuffer){};
        virtual void transferImageToRender(VkCommandBuffer){};
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
        virtual common::Resource<swizzle::gfx::Texture> getDepthAttachment() override;

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

        VkFormat getFormat(swizzle::gfx::FrameBufferAttachmentType type);

        common::Resource<Device> mDevice;
        const swizzle::gfx::FrameBufferCreateInfo mCreateInfo;

        VkRenderPass mRenderPass;
        VkFramebuffer mFrameBuffer;

        U32 mWidth;
        U32 mHeight;

        SwBool mHasDepthBuffer;
        VFrameBufferImage mDepthImage;

        std::vector<VFrameBufferImage> mImages;

        common::Resource<swizzle::gfx::Material> mMaterial;
    };
} // namespace vk

/* Function Declaration */

namespace vk
{
    common::Resource<BaseFrameBuffer>
        GetFrameBufferAsBaseFrameBuffer(common::Resource<swizzle::gfx::FrameBuffer> buffer);
}

#endif