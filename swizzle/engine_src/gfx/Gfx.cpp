
#include <swizzle/core/logging/Logging.hpp>
#include "Gfx.hpp"

#include "gfxvk/VulkanRenderer.hpp"
#include "gfxvk/VulkanSwapchain.hpp"
#include "gfxvk/VulkanCmdBuffer.hpp"
#include "gfxvk/VulkanBuffer.hpp"
#include "gfxvk/VulkanTexture.hpp"

namespace swizzle::gfx
{

    VulkanRenderer* gRenderInstance = nullptr;

    SwBool GfxInitialize()
    {
        LOG_INFO("Initializing Graphics!");
        gRenderInstance = new VulkanRenderer();

        return true;
    }

    SwBool GfxCleanup()
    {
        vkDeviceWaitIdle(gRenderInstance->getVkObjects().mLogicalDevice);
        LOG_INFO("Shutting down Graphics!");

        delete gRenderInstance;
        gRenderInstance = nullptr;

        return true;
    }

    core::Resource<Buffer> CreateBuffer(BufferType type)
    {
        return core::CreateRef<VulkanBuffer>(gRenderInstance->getVkObjects(), type);
    }

    core::Resource<CommandBuffer> CreateCommandBuffer()
    {
        return core::CreateRef<VulkanCommandBuffer>(gRenderInstance->getVkObjects());
    }

    core::Resource<FrameBuffer> CreateFrameBuffer()
    {
        return nullptr;
    }

    core::Resource<Swapchain> CreateSwapchain(core::Resource<core::Window> window)
    {
        return core::CreateRef<VulkanSwapchain>(window, gRenderInstance->getVkObjects());;
    }

    core::Resource<Texture> CreateTexture()
    {
        return core::CreateRef<VulkanTexture>(gRenderInstance->getVkObjects());
    }

    void WaitIdle()
    {
        vkDeviceWaitIdle(gRenderInstance->getVkObjects().mLogicalDevice);
    }

}