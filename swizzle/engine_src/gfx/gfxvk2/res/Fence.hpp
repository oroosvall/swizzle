#ifndef FENCE_HPP
#define FENCE_HPP

/* Include files */

#include "_fwDecl.hpp"
#include "../backend/Vk.hpp"

#include <vector>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{
    class Fence
    {
    public:
        Fence(common::Resource<Device> device);
        ~Fence();

        SwBool isInUse() const;
        void resetFence();

        VkFence getHandle() const;

        void addUser(std::weak_ptr<CmdBuffer> user);
        void removeUser(std::weak_ptr<CmdBuffer> user);

    private:
        common::Resource<Device> mDevice;

        VkFence mFence;

        std::vector<std::weak_ptr<CmdBuffer>> mUsers;

    };
}

/* Function Declaration */

#endif
