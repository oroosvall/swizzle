
/* Include files */

#include "Fence.hpp"

#include "Device.hpp"

#include <algorithm>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

template <typename T, typename U>
inline bool equals(const std::weak_ptr<T>& t, const std::weak_ptr<U>& u)
{
    return !t.owner_before(u) && !u.owner_before(t);
}

/* Class Public Function Definition */

namespace vk
{
    Fence::Fence(common::Resource<Device> device)
        : mDevice(device)
        , mFence(VK_NULL_HANDLE)
        , mUsers()
    {
        mFence = mDevice->createFence(vk::FenceCreateFlags::signaled);
    }

    Fence::~Fence()
    {
        mDevice->destroyFence(mFence);
    }

    SwBool Fence::isInUse() const
    {
        return !mUsers.empty();
    }

    void Fence::resetFence()
    {
        vkResetFences(mDevice->getDeviceHandle(), 1, &mFence);
    }

    VkFence Fence::getHandle() const
    {
        return mFence;
    }

    void Fence::addUser(std::weak_ptr<CmdBuffer> user)
    {
        mUsers.push_back(user);
    }

    void Fence::removeUser(std::weak_ptr<CmdBuffer> user)
    {
        mUsers.erase(
            std::remove_if(mUsers.begin(), mUsers.end(),
                [&user](const auto& obj) { return equals(obj, user); }),
            mUsers.end());
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */
