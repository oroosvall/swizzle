#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include <swizzle/core/Types.hpp>
#include <swizzle/core/Resource.hpp>

#include <swizzle/gfx/Shader.hpp>

namespace swizzle::gfx
{

    struct ClearColor
    {
        F32 r;
        F32 g;
        F32 b;
        F32 a;
    };

    class FrameBuffer
    {
    public:

        virtual ~FrameBuffer() {}

        virtual U32 getNumColorAttachments() const = 0;
        virtual SwBool hasDepthAttachment() const = 0;

        virtual void setColorAttachmentClearColor(U32 attachmentIndex, ClearColor color) = 0;
        virtual void setDepthAttachmentClearValue(F32 depthValue, U8 stencilValue) = 0;

        virtual void resize(U32 width, U32 height) = 0;

        virtual core::Resource<Shader> createShader(const ShaderAttributeList& attributeList) = 0;

    };
}

#endif