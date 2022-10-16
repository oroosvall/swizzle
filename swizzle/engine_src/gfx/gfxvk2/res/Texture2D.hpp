#ifndef TEXTURE_2D_HPP
#define TEXTURE_2D_HPP

/* Include files */

#include "TextureBase.hpp"

#include "../backend/Vk.hpp"
#include "_fwDecl.hpp"

#include "VkResource.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{
    class Texture2D : public TextureBase
    {
    public:
        Texture2D(common::Resource<Device> device, U32 width, U32 height, U32 channels, const U8* pixelData);
        virtual ~Texture2D();

        virtual void setData(U32 width, U32 height, U32 channels, const U8* pixelData) override;
        virtual void upload() override;
        virtual void getTextureSize(U32& w, U32& h) override;

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
    };
} // namespace vk

/* Function Declaration */

#endif