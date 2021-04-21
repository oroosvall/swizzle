#ifndef VULKAN_CUBE_MAP_HPP
#define VULKAN_CUBE_MAP_HPP

/* Include files */

#include <swizzle/gfx/Texture.hpp>
#include "VulkanTexture.hpp"

#include "backend/Vk.hpp"
#include "backend/VkContainer.hpp"


/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::gfx
{
    class VulkanCubeMap : public VulkanTextureIfc
    {
    public:

        VulkanCubeMap(const VkContainer vkObjects, U32 width, U32 height, U32 channels, const U8* pixelData);
        virtual ~VulkanCubeMap();

        // Inherited via Texture
        virtual void setData(U32 width, U32 height, U32 channels, const U8* pixelData) override;

        virtual void upload() override;

        VkImageView getView() const;

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
        U32 mLayers;

    };

}

#endif

/* Function Declaration */
