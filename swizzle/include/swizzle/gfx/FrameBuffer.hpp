#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

/* Include files */

#include <common/Common.hpp>

#include <swizzle/gfx/Shader.hpp>
#include <swizzle/gfx/Material.hpp>

/* Defines */

/* Typedefs/enums */

namespace swizzle::gfx
{
    enum class FrameBufferDepthType
    {
        DepthNone,
        DepthOnly,
        DepthStencil,
    };
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::gfx
{
    struct ClearColor
    {
        F32 r;
        F32 g;
        F32 b;
        F32 a;
    };

    struct FrameBufferCreateInfo
    {
        U32 mWidth;
        U32 mHeight;

        U32 mSwapCount;
        U32 mNumColAttach;
        FrameBufferDepthType mDepthType;
    };
}

/* Class Declaration */

namespace swizzle::gfx
{
    class FrameBuffer
    {
    public:

        virtual ~FrameBuffer() {}

        virtual U32 getNumColorAttachments() const = 0;
        virtual SwBool hasDepthAttachment() const = 0;

        virtual void setColorAttachmentClearColor(U32 attachmentIndex, ClearColor color) = 0;
        virtual void setDepthAttachmentClearValue(F32 depthValue, U8 stencilValue) = 0;

        virtual common::Resource<Texture> getColorAttachment(U32 index) = 0;

        virtual void resize(U32 width, U32 height) = 0;

        virtual common::Resource<Shader> createShader(const ShaderAttributeList& attributeList) = 0;

    };
}

/* Function Declaration */


#endif