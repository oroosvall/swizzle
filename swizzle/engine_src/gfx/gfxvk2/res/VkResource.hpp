#ifndef VK_RESOURCE_HPP
#define VK_RESOURCE_HPP

/* Include files */

#include <common/Common.hpp>
#include "_fwDecl.hpp"
#include "../backend/Vk.hpp"

#include "CmdBuffer.hpp"

#include <vector>

/* Defines */

/* Typedefs/enums */

namespace vk
{
    enum class ResourceType
    {
        BufferResource,
        ImageResource,
    };
} // namespace vk

/* Forward Declared Structs/Classes */

namespace vk
{
    typedef std::weak_ptr<CmdBuffer> WeakCmdBuffer;
    typedef std::pair<WeakCmdBuffer, U64> FrameUser;
    typedef std::vector<FrameUser> FrameResourceUser;
} // namespace vk

/* Struct Declaration */

/* Class Declaration */

namespace vk
{
    class IVkResource
    {
    public:
        virtual ~IVkResource() {}
        virtual ResourceType getType() const = 0;
        virtual U32 activeUserCount() = 0;
    };

    template<typename T>
    class VkResource : public IVkResource
    {
    public:
        VkResource(T resource, ResourceType resourceType)
            : mResource(resource)
            , mResourceType(resourceType)
        {}

        virtual ~VkResource() noexcept
        {
            if (mResource)
            {
                LOG_ERROR("VkResource: A vulkan object has been leaked, Object %p", mResource);
            }
        }

        void addUser(common::Resource<CmdBuffer> buffer, U64 frameCount)
        {
            FrameUser& found = findOrCreateUser(buffer);
            found.second = frameCount;
        }

        T reset()
        {
            T ret = mResource;
            mResource = VK_NULL_HANDLE;
            return ret;
        }

        T getVkHandle() const
        {
            return mResource;
        }

        ResourceType getType() const
        {
            return mResourceType;
        }

        U32 activeUserCount()
        {
            U32 count = 0u;

            for (const auto& p : mCommandBufferUsers)
            {
                U64 frameCount = p.second;
                auto cmd = p.first.lock();
                if (cmd)
                {
                    if (cmd->isFrameInProgress(frameCount))
                    {
                        count++;
                    }
                }
            }

            return count;
        }

        explicit operator SwBool()
        {
            return mResource != VK_NULL_HANDLE;
        }

    private:
        FrameUser& findOrCreateUser(common::Resource<CmdBuffer> buffer)
        {
            // @TODO: needs loock when accross threads
            size_t index = ~0ull;
            size_t ctr = 0ull;

            for (const auto& p : mCommandBufferUsers)
            {
                if (p.first.lock().get() == buffer.get())
                {
                    index = ctr;
                    break;
                }
                ctr++;
            }

            if (index == ~0ull)
            {
                mCommandBufferUsers.emplace_back(FrameUser(buffer, 0ull));
                index = mCommandBufferUsers.size() - 1ull;
            }

            return mCommandBufferUsers[index];
        }

        T mResource;
        ResourceType mResourceType;
        FrameResourceUser mCommandBufferUsers;
    };
} // namespace vk

/* Function Declaration */


#endif