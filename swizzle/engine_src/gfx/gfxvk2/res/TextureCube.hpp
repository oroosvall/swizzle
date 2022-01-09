#ifndef TEXTURE_CUBE_HPP
#define TEXTURE_CUBE_HPP

/* Include files */

#include "TextureBase.hpp"

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
    class TextureCube : public TextureBase
    {
    public:
        TextureCube(common::Resource<Device> device, U32 width, U32 height, U32 channels, const U8* pixelData);
        virtual ~TextureCube();

        virtual void setData(U32 width, U32 height, U32 channels, const U8* pixelData) override;
        virtual void upload() override;

        virtual SwBool isUploaded() const override;
        virtual void uploadImage(VkCommandBuffer cmdBuffer) override;

        virtual common::Resource<VkResource<VkImage>> getImg() override;
        virtual VkImageView getView() override;

    private:
        void createImage(VkFormat format);
        void allocMemory();
        void createView(VkFormat format);

        void destroyResources();
        void createResources();

        common::Resource<Device> mDevice;

        common::Resource<VkResource<VkImage>> mImage;
        common::Resource<DeviceMemory> mImageMemory;

        VkImageView mImageView;

        SwBool mUploaded;

        common::Resource<VkResource<VkBuffer>> mStageBuffer;
        common::Resource<DeviceMemory> mStageMemory;

        U32 mWidth;
        U32 mHeight;
        U32 mChannels;
        U32 mLayers;
    };
} // namespace vk

/* Function Declaration */

#endif
