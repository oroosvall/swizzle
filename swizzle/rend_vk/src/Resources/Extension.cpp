
/* Include files */

#include "Extension.hpp"
#include "Instance.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace rvk
{
    Extensions::Extensions(std::shared_ptr<Instance> inst, PhysicalDeviceFeatures features)
    {
        if (features.mMeshShaders.meshShader && features.mMeshShaders.taskShader)
        {
            mPfnDrawMeshTask = reinterpret_cast<PFN_vkCmdDrawMeshTasksEXT>(
                vkGetInstanceProcAddr(inst->getHandle(), "vkCmdDrawMeshTasksEXT"));
            mPfnDrawMeshTaskIndirect = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectEXT>(
                vkGetInstanceProcAddr(inst->getHandle(), "vkCmdDrawMeshTasksIndirectEXT"));
            mPfnDrawMeshTaskIndirectCount = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectCountEXT>(
                vkGetInstanceProcAddr(inst->getHandle(), "vkCmdDrawMeshTasksIndirectCountEXT"));
        }
    }

    Extensions::~Extensions()
    {
    }

    bool Extensions::hasMeshShader() const
    {
        return (mPfnDrawMeshTask && mPfnDrawMeshTaskIndirect && mPfnDrawMeshTaskIndirectCount);
    }

} // namespace rvk

/* Class Protected Function Definition */

/* Class Private Function Definition */
