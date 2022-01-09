#ifndef VULKAN_TEXTURE_HPP
#define VULKAN_TEXTURE_HPP

#include <swizzle/gfx/Texture.hpp>

#include "backend/Vk.hpp"
#include "backend/VkContainer.hpp"

namespace swizzle::gfx
{

    class VulkanTextureIfc : public Texture
    {
    public:
        virtual ~VulkanTextureIfc() {}

        virtual SwBool isUploaded() const = 0;
        virtual void uploadImage(VkCommandBuffer cmdBuffer) = 0;
    };

    class VulkanTexture : public VulkanTextureIfc
    {
    public:

        VulkanTexture(const VkContainer vkObjects, U32 width, U32 height, U32 channels, const U8* pixelData);
        virtual ~VulkanTexture();

        // Inherited via Texture
        virtual void setData(U32 width, U32 height, U32 channels, const U8* pixelData) override;

        virtual void upload() override;

        VkImageView getView() const { return mImageView; }

        virtual SwBool isUploaded() const override;

        virtual void uploadImage(VkCommandBuffer cmdBuffer) override;

    private:

        void createImage(VkFormat format);
        void allocMemory();
        void createView(VkFormat format);

        void destroyResources();
        void createResources();

        const VkContainer mVkObjects;

        VkImage mImage;
        VkImageView mImageView;

        VkDeviceMemory mMemory;

        SwBool mUploaded;

        VkBuffer mStageBuffer;
        VkDeviceMemory mStageMemory;

        U32 mWidth;
        U32 mHeight;
        U32 mChannels;

    };

}

#endif