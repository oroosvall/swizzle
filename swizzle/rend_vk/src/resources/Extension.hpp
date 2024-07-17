
#pragma once

/* Include files */

#include "Vk.hpp"
#include "_fw.hpp"

#include <memory>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace rvk
{
    class Extensions
    {
    public:
        Extensions(std::shared_ptr<Instance> inst, PhysicalDeviceFeatures features);
        virtual ~Extensions();

        bool hasMeshShader() const;

    private:

        PFN_vkCmdDrawMeshTasksEXT mPfnDrawMeshTask = nullptr;
        PFN_vkCmdDrawMeshTasksIndirectEXT mPfnDrawMeshTaskIndirect = nullptr;
        PFN_vkCmdDrawMeshTasksIndirectCountEXT mPfnDrawMeshTaskIndirectCount = nullptr;

    };
} // namespace rvk

/* Function Declaration */