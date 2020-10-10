#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include <swizzle/core/common/Types.hpp>
#include <swizzle/core/common/Resource.hpp>

#include <swizzle/gfx/Shader.hpp>

namespace swizzle::gfx
{

    struct ClearColor
    {
        SwFloat r;
        SwFloat g;
        SwFloat b;
        SwFloat a;
    };

    class FrameBuffer
    {
    public:

        virtual ~FrameBuffer() {}

        virtual U32 getNumColorAttachments() const = 0; 
        virtual SwBool hasDepthAttachment() const = 0;

        virtual void setColorAttachmentClearColor(U32 attachmentIndex, ClearColor color) = 0;
        virtual void setDepthAttachmentClearValue(SwFloat depthValue, U8 stencilValue) = 0;

        virtual void resize(U32 width, U32 height) = 0;

        virtual core::Resource<Shader> createShader(ShaderAttributeList attributeList) = 0;

    };
}

#endif