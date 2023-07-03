#ifndef TEXTURE_CUBE_HPP
#define TEXTURE_CUBE_HPP

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
    class TextureCube : public TextureBase
    {
    public:
        TextureCube(common::Resource<Device> device, U32 width, U32 height, U32 channels);
        virtual ~TextureCube();

        virtual swizzle::gfx::TextureDimensions getSize() const override;
        void resize(U32 height, U32 width, U32 channels) override;

        virtual void transferImageToCompute(VkCommandBuffer cmdBuffer) override;
        virtual void transferImageToRender(VkCommandBuffer cmdBuffer) override;

        virtual common::Resource<VkResource<VkImage>> getImg() override;
        virtual VkImageView getView() override;
        virtual bool isDepth() override;

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

        U32 mWidth;
        U32 mHeight;
        U32 mChannels;
        U32 mLayers;
    };
} // namespace vk

/* Function Declaration */

#endif
