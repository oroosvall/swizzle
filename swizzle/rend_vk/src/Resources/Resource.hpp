
#pragma once

/* Include files */

#include "Vk.hpp"
#include "_fw.hpp"

#include <memory>

#include "../Log.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace rvk
{

    class IResource
    {
    public:
        virtual ~IResource() = default;

        virtual uint32_t getUserCount() const                                   = 0;
        virtual void addUser()                                                  = 0;
        virtual void removeUser()                                               = 0;
        virtual void bindMemory(std::shared_ptr<Device> dev, DeviceMemory* mem) = 0;
    };

    template <typename T>
    class Resource : public IResource
    {
    public:
        Resource(T rsc) noexcept
            : mResource(rsc)
            , mUsers(0ul)
        {
        }

        Resource(Resource& org) = delete;

        Resource(Resource&& org) noexcept
            : mResource(std::move(org.mResource))
            , mUsers(org.mUsers)
        {
            org.mResource = VK_NULL_HANDLE;
            org.mUsers    = 0ul;
        }

        virtual ~Resource()
        {
            if (mResource)
            {
                LogMessage(LogLevel::Error, "Resource of type %s: @%p was leaked", typeid(mResource).name(), mResource);
            }
        }

        virtual void bindMemory(std::shared_ptr<Device> dev, DeviceMemory* mem) override;

        virtual uint32_t getUserCount() const override
        {
            return mUsers;
        }

        virtual void addUser() override
        {
            mUsers++;
        }

        virtual void removeUser() override
        {
            mUsers--;
        }

        operator T() const
        {
            return mResource;
        }

    private:
        T mResource;
        uint32_t mUsers;
    };

} // namespace rvk

/* Function Declaration */