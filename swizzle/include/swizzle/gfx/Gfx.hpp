#ifndef GFX_HPP
#define GFX_HPP

#include <common/Resource.hpp>
#include <swizzle/core/Window.hpp>
#include <swizzle/Api.hpp>

#include "Buffer.hpp"
#include "Color.hpp"
#include "CommandBuffer.hpp"
#include "FrameBuffer.hpp"
#include "Swapchain.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

namespace swizzle::gfx
{

    core::Resource<Buffer> SWIZZLE_API CreateBuffer(BufferType type);
    core::Resource<CommandBuffer> SWIZZLE_API CreateCommandBuffer();
    core::Resource<FrameBuffer> SWIZZLE_API CreateFrameBuffer();
    core::Resource<Swapchain> SWIZZLE_API CreateSwapchain(core::Resource<core::Window> window);
    core::Resource<Texture> SWIZZLE_API CreateTexture(U32 width, U32 height);

    void SWIZZLE_API WaitIdle();

}

#endif