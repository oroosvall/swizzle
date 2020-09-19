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
        virtual void setData(SwU32 width, SwU32 height, SwU32 channels, SwU8* pixelData) override;

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

        SwU32 mWidth;
        SwU32 mHeight;

    };

}

#endif