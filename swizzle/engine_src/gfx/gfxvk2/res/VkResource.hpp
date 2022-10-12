#ifndef VK_RESOURCE_HPP
#define VK_RESOURCE_HPP

/* Include files */

#include "../backend/Vk.hpp"
#include "_fwDecl.hpp"
#include <common/Common.hpp>

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
        DescriptorResource
    };
} // namespace vk

/* Forward Declared Structs/Classes */

namespace vk
{
    typedef std::weak_ptr<CmdBuffer> WeakCmdBuffer;
    typedef std::pair<WeakCmdBuffer, U64> FrameUser;
    typedef std::vector<FrameUser> FrameResourceUser;

    typedef std::weak_ptr<LifetimeToken> WeakLifetimeToken;
    typedef std::pair<WeakLifetimeToken, U64> LifetimeUser;
    typedef std::vector<LifetimeUser> LifetimeResourceUser;

} // namespace vk

/* Struct Declaration */

/* Class Declaration */

namespace vk
{

    class LifetimeToken
    {
    public:
        LifetimeToken(U64 checkpoint, U64 count)
            : mCheckpoint(checkpoint)
            , mCount(count)
        {
        }

        ~LifetimeToken() {}

        void incrementCheckpoint()
        {
            mCheckpoint++;
        }

        SwBool isActive(U64 checkpoint) const
        {
            SwBool active = true;
            if ((checkpoint + mCount + 1u) < mCheckpoint)
            {
                active = false;
            }
            else
            {
                // TODO: Check agains synchronization object
            }
            return active;
        }

        U64 getCheckpoint() const
        {
            return mCheckpoint;
        }

    private:
        U64 mCheckpoint;
        U64 mCount;
    };

    class IVkResource
    {
    public:
        virtual ~IVkResource() {}
        virtual ResourceType getType() const = 0;
        virtual U32 activeUserCount() = 0;
    };

    template <typename T> class VkResource : public IVkResource
    {
    public:
        VkResource(T resource, ResourceType resourceType)
            : mResource(resource)
            , mResourceType(resourceType)
        {
        }

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

        void addUser(common::Resource<LifetimeToken> token, U64 checkpoint)
        {
            LifetimeUser& found = findOrCreateUser(token);
            found.second = checkpoint;
        }

        void addResourceDependency(common::Resource<IVkResource> resource)
        {
            addNewResourceDependencyIfNotFound(resource);
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

            for (const auto& p : mLifetimeTokenUsers)
            {
                U64 checkpoint = p.second;
                auto cmd = p.first.lock();
                if (cmd)
                {
                    if (cmd->isActive(checkpoint))
                    {
                        count++;
                    }
                }
            }

            for (const auto& dep : mResourceDependencies)
            {
                count += dep->activeUserCount();
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

        LifetimeUser& findOrCreateUser(common::Resource<LifetimeToken> token)
        {
            // @TODO: needs loock when accross threads
            size_t index = ~0ull;
            size_t ctr = 0ull;

            for (const auto& p : mLifetimeTokenUsers)
            {
                if (p.first.lock().get() == token.get())
                {
                    index = ctr;
                    break;
                }
                ctr++;
            }

            if (index == ~0ull)
            {
                mLifetimeTokenUsers.emplace_back(LifetimeUser(token, 0ull));
                index = mLifetimeTokenUsers.size() - 1ull;
            }

            return mLifetimeTokenUsers[index];
        }

        void addNewResourceDependencyIfNotFound(common::Resource<IVkResource> resource)
        {
            size_t index = ~0ull;
            size_t ctr = 0ull;

            for (const auto& p : mResourceDependencies)
            {
                if (p.get() == resource.get())
                {
                    index = ctr;
                    break;
                }
                ctr++;
            }

            if (index == ~0ull)
            {
                mResourceDependencies.emplace_back(resource);
            }
        }

        T mResource;
        ResourceType mResourceType;
        FrameResourceUser mCommandBufferUsers;
        LifetimeResourceUser mLifetimeTokenUsers;
        std::vector<common::Resource<IVkResource>> mResourceDependencies;
    };
} // namespace vk

/* Function Declaration */

#endif