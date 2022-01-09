#ifndef VK_RESOURCE_TRACKER_HPP
#define VK_RESOURCE_TRACKER_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/core/Logging.hpp>
#include "Vk.hpp"

#include <list>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{

    template<typename T>
    struct Descriptor
    {
        T mResource;
    };

    template<typename T>
    class VkResource
    {
    public:

        VkResource(T res)
            : mVkResourceHandle(res)
            , mCommandBufferUsers()
            , mDescriptorUsers()
        {}

        ~VkResource() noexcept
        {
            if (mVkResourceHandle)
            {
                LOG_ERROR("VkOwner: A vulkan object has been leaked, Object %p", mVkResourceHandle);
            }
        }

        VkResource(VkResource<T>&& rhs) noexcept
            : mVkResourceHandle(rhs.mVkResourceHandle)
            , mCommandBufferUsers(std::move(rhs.mCommandBufferUsers))
            , mDescriptorUsers(std::move(rhs.mDescriptorUsers))
        {
            rhs.mVkResourceHandle = VK_NULL_HANDLE;
            rhs.mCommandBufferUsers.clear();
            rhs.mDescriptorUsers.clear();
        }

        void addReference(std::weak_ptr<VkCommandBuffer> cmdBuffer, U32 submitNumber)
        {
            mCommandBufferUsers[cmdBuffer] = submitNumber;
        }

        /* This exists to make cleanup easier */
        void removeReferenceIfExist(std::weak_ptr<VkCommandBuffer> cmdBuffer)
        {
            mCommandBufferUsers.erase(cmdBuffer);
        }

        SwBool readyForCleanup() const
        {
            SwBool cleanup = false;
            if (mCommandBufferUsers.empty() && mDescriptorUsers.empty())
            {
                cleanup = true;
            }
            else
            {
                for (const auto& p : mCommandBufferUsers)
                {
                    common::Resource<VkCommandBuffer> cmd = p.first.lock();
                }
            }

            return cleanup;
        }

        VkResource& operator=(VkResource<T>&& rhs) noexcept
        {
            if (mVkResourceHandle)
            {
                LOG_ERROR("VkOwner: A vulkan object has been leaked, Object %p", mVkResourceHandle);
            }

            mVkResourceHandle = rhs.mVkResourceHandle;
            rhs.mVkResourceHandle = VK_NULL_HANDLE;
            return *this;
        }

        VkResource(const VkResource&) = delete;
        VkResource& operator=(const VkResource&) = delete;

    private:
        T mVkResourceHandle;

        std::list<std::pair<std::weak_ptr<VkCommandBuffer>, U32>> mCommandBufferUsers;
        std::vector<std::weak_ptr<Descriptor<VkBuffer>>> mDescriptorUsers;



    };

    template<typename T>
    class VkResourceView
    {
    public:

    private:
        T mVkResourceHandle;
    };

    class VkMemory
    {
    public:
        VkMemory(VkDeviceMemory mem)
            : mMemory(mem)
            , mMemoryUsers(0u) {}

        VkDeviceMemory operator()() const
        {
            return mMemory;
        }

    private:
        VkDeviceMemory mMemory;
        U32 mMemoryUsers;
    };

}

/* Function Declaration */


#endif