#ifndef TEXTURE_BASE_HPP
#define TEXTURE_BASE_HPP

/* Include files */

#include <swizzle/gfx/Texture.hpp>

#include "_fwDecl.hpp"

#include "VkResource.hpp"
#include "../backend/Vk.hpp"

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

        virtual SwBool isUploaded() const = 0;
        virtual void uploadImage(VkCommandBuffer cmdBuffer) = 0;

        virtual common::Resource<VkResource<VkImage>> getImg() = 0;
        virtual bool isDepth() = 0;
        virtual VkImageView getView() = 0;

        virtual void transferImageToCompute(VkCommandBuffer cmdBuffer) = 0;
        virtual void transferImageToRender(VkCommandBuffer cmdBuffer) = 0;
    };
}

/* Function Declaration */


#endif