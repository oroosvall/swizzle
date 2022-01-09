#ifndef CMD_POOL_HPP
#define CMD_POOL_HPP

/* Include files */

#include "_fwDecl.hpp"
#include "../backend/Vk.hpp"

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{
    class CmdPool
    {
    public:
        CmdPool(common::Resource<vk::Device> device, U32 queueFamily);
        virtual ~CmdPool();

        VkCommandBuffer* allocateCmdBuffer(U32 cmdBufferCount, VkCommandBufferLevel level);
        void freeCmdBuffer(VkCommandBuffer* cmdBuffers, U32 cmdBufferCount);

        void resetPool();

    private:
        common::Resource<vk::Device> mDevice;
        VkCommandPool mCommandPool;
    };
}

/* Function Declaration */

#endif