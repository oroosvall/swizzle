#ifndef TEXTURE_BASE_HPP
#define TEXTURE_BASE_HPP

/* Include files */

#include <swizzle/gfx/Texture.hpp>

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
    class TextureBase : public swizzle::gfx::Texture
    {
    public:
        virtual ~TextureBase(){};

        virtual void resize(U32 height, U32 width, U32 channels) = 0;

        virtual common::Resource<VkResource<VkImage>> getImg() = 0;
        virtual bool isDepth() = 0;
        virtual VkImageView getView() = 0;

        virtual void transferImageToCompute(VkCommandBuffer cmdBuffer) = 0;
        virtual void transferImageToRender(VkCommandBuffer cmdBuffer) = 0;
    };
} // namespace vk

/* Function Declaration */

namespace vk
{
    void uploadTexture(VkCommandBuffer cmdBuffer, VkBuffer data, VkImage image,
                       const swizzle::gfx::TextureDimensions& info);
    void generateMipMaps(VkCommandBuffer cmdBuffer, VkImage image, const swizzle::gfx::TextureDimensions& info);
} // namespace vk

#endif