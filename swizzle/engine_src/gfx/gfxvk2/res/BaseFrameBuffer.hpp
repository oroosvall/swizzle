#ifndef BASE_FRAME_BUFFER_HPP
#define BASE_FRAME_BUFFER_HPP

/* Include files */

#include <swizzle/gfx/FrameBuffer.hpp>

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
    class BaseFrameBuffer : public swizzle::gfx::FrameBuffer
    {
    public:
        virtual ~BaseFrameBuffer() { }

        virtual VkRenderPass getRenderPass() const = 0;
        virtual VkSampleCountFlagBits getMultisampleCount() const = 0;
        virtual VkFramebuffer getFrameBuffer() const = 0;

        virtual void getSize(U32& width, U32& height) const = 0;
        virtual VkClearValue getColorClearValue(U32 index) const = 0;
        virtual VkClearValue getDepthClearValue() const = 0;

    };
}

/* Function Declaration */

#endif
