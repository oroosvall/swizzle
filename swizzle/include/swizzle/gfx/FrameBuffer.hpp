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

        virtual SwU32 getNumColorAttachments() const = 0; 
        virtual SwBool hasDepthAttachment() const = 0;

        virtual void setColorAttachmentClearColor(SwU32 attachmentIndex, ClearColor color) = 0;
        virtual void setDepthAttachmentClearValue(SwFloat depthValue, SwU8 stencilValue) = 0;

        virtual void resize(SwU32 width, SwU32 height) = 0;

        virtual core::Resource<Shader> createShader(ShaderAttributeList attributeList) = 0;

    };
}

#endif