#ifndef VULKAN_TEXTURE_HPP
#define VULKAN_TEXTURE_HPP

#include <swizzle/gfx/Texture.hpp>

#include "vk.hpp"
#include "VulkanObjectContainer.hpp"

namespace swizzle::gfx
{

    class VulkanTexture : public Texture
    {
    public:

        VulkanTexture(const VulkanObjectContainer& vkObjects);
        virtual ~VulkanTexture();

        // Inherited via Texture
        virtual void setData(U32 width, U32 height, U32 channels, U8* pixelData) override;

        VkImageView getView() const { return mImageView; }

        
    private:

        void createImage();
        void allocMemory();
        void createView();

        void destroyResources();
        void createResources();

        const VulkanObjectContainer& mVkObjects;

    public: // This is a temporary hack
        VkImage mImage;
        VkImageView mImageView;

        VkDeviceMemory mMemory;

        SwBool uploaded;

        VkDeviceMemory mStageMemory;
        VkBuffer mStageBuffer;

        U32 mWidth;
        U32 mHeight;

    };

}

#endif