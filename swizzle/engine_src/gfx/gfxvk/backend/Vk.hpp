#ifndef VK_HPP
#define VK_HPP

#include <common/Types.hpp>

#if defined(SW_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#pragma warning(push)
#pragma warning(disable : 26495)
#pragma warning(disable : 26812)
#include <vulkan/vulkan.h>
#pragma warning(pop)

#include <swizzle/core/Logging.hpp>

namespace vk
{
    const char* VkResultToString(VkResult res);

    // This is a naive implementation
    // This will not clean up any resources from vulkan
    // This object must be handed to VulkanLogical xxxDelete function
    // for cleanup
    template<typename T>
    class VkOwner
    {
    public:
        VkOwner(T obj) : mObject(obj) {}
        ~VkOwner() noexcept
        {
            if (mObject)
            {
                LOG_ERROR("VkOwner: A vulkan object has been leaked, Object %p", mObject);
            }
        }

        VkOwner(VkOwner<T>&& rhs) noexcept
            : mObject(rhs.mObject)
        {
            rhs.mObject = VK_NULL_HANDLE;
        }

        VkOwner& operator=(VkOwner<T>&& rhs) noexcept
        {
            if (mObject)
            {
                LOG_ERROR("VkOwner: A vulkan object has been leaked, Object %p", mObject);
            }

            mObject = rhs.mObject;
            rhs.mObject = VK_NULL_HANDLE;
            return *this;
        }

        inline T get() const noexcept
        {
            return mObject;
        }

        T release() noexcept
        {
            auto o = mObject;
            mObject = VK_NULL_HANDLE;
            return o;
        }

        explicit operator bool() const noexcept
        {
            return static_cast<bool>(mObject);
        }

        VkOwner(const VkOwner&) = delete;
        VkOwner& operator=(const VkOwner&) = delete;

    private:
        T mObject;
    };
}

#endif